#include "PlySplatLoader.h"

THIRD_PARTY_INCLUDES_START
#include "tinyply.h"
THIRD_PARTY_INCLUDES_END

#include <fstream>
#include <vector>

using namespace tinyply;

namespace
{
	template<typename T>
	static void CopyToVector(const std::shared_ptr<PlyData>& Data, std::vector<T>& Out)
	{
		if (!Data) return;

		const size_t Bytes = Data->buffer.size_bytes();
		const size_t N = Bytes / sizeof(T);

		Out.resize(N);
		std::memcpy(Out.data(), Data->buffer.get(), Bytes);
	}

	// Convert any numeric PlyData to float vector.
	static void ToFloatVector(const std::shared_ptr<PlyData>& Data, std::vector<float>& Out)
	{
		if (!Data) return;

		switch (Data->t)
		{
		case Type::FLOAT32:
		{
			CopyToVector(Data, Out);
			return;
		}
		case Type::FLOAT64:
		{
			std::vector<double> tmp;
			CopyToVector(Data, tmp);
			Out.resize(tmp.size());
			for (size_t i = 0; i < tmp.size(); ++i) Out[i] = (float)tmp[i];
			return;
		}
		case Type::INT32:
		{
			std::vector<int32_t> tmp;
			CopyToVector(Data, tmp);
			Out.resize(tmp.size());
			for (size_t i = 0; i < tmp.size(); ++i) Out[i] = (float)tmp[i];
			return;
		}
		case Type::UINT8:
		{
			std::vector<uint8_t> tmp;
			CopyToVector(Data, tmp);
			Out.resize(tmp.size());
			for (size_t i = 0; i < tmp.size(); ++i) Out[i] = (float)tmp[i];
			return;
		}
		default:
			return;
		}
	}

	static std::shared_ptr<PlyData> TryRequest(PlyFile& File, const std::string& Element, const std::initializer_list<std::string>& Props)
	{
		try { return File.request_properties_from_element(Element, Props); }
		catch (...) { return nullptr; }
	}

	static bool Has(const std::vector<float>& v, size_t n) { return v.size() >= n; }
}

bool FPlySplatLoader::LoadSplatsFromPly(const FString& FilePath, TArray<FSplat>& OutSplats, FString& OutError)
{
	OutSplats.Reset();
	OutError.Reset();

	std::ifstream Stream(TCHAR_TO_UTF8(*FilePath), std::ios::binary);
	if (!Stream)
	{
		OutError = TEXT("Failed to open file.");
		return false;
	}

	try
	{
		PlyFile File;
		File.parse_header(Stream);

		// Positions (required)
		auto v_xyz = TryRequest(File, "vertex", {"x","y","z"});
		if (!v_xyz)
		{
			OutError = TEXT("PLY has no vertex x/y/z.");
			return false;
		}

		// Optional properties (try common names)
		auto v_scale = TryRequest(File, "vertex", {"scale_0","scale_1","scale_2"});
		if (!v_scale) v_scale = TryRequest(File, "vertex", {"scales_0","scales_1","scales_2"});

		auto v_rot = TryRequest(File, "vertex", {"rot_0","rot_1","rot_2","rot_3"});
		if (!v_rot) v_rot = TryRequest(File, "vertex", {"rotation_0","rotation_1","rotation_2","rotation_3"});

		auto v_fdc = TryRequest(File, "vertex", {"f_dc_0","f_dc_1","f_dc_2"});
		auto v_rgb = TryRequest(File, "vertex", {"red","green","blue"});

		auto v_opacity = TryRequest(File, "vertex", {"opacity"});

		File.read(Stream);

		std::vector<float> xyz, scale, rot, fdc, rgb, opacity;
		ToFloatVector(v_xyz, xyz);
		ToFloatVector(v_scale, scale);
		ToFloatVector(v_rot, rot);
		ToFloatVector(v_fdc, fdc);
		ToFloatVector(v_rgb, rgb);
		ToFloatVector(v_opacity, opacity);

		const size_t Count = xyz.size() / 3;
		if (Count == 0 || xyz.size() % 3 != 0)
		{
			OutError = TEXT("Invalid x/y/z data.");
			return false;
		}

		OutSplats.Reserve((int32)Count);

		for (size_t i = 0; i < Count; ++i)
		{
			FSplat S;

			const float x = xyz[i*3+0];
			const float y = xyz[i*3+1];
			const float z = xyz[i*3+2];

			// NOTE: many splat PLYs are in meters; UE is cm. Decide your convention.
			S.Position = FVector(x, y, z) * 100.0f;

			if (Has(scale, (i+1)*3))
			{
				S.Scale = FVector(scale[i*3+0], scale[i*3+1], scale[i*3+2]);
			}

			if (Has(rot, (i+1)*4))
			{
				// Some datasets store (x,y,z,w). If your PLY uses (w,x,y,z), swap here.
				const float qx = rot[i*4+0];
				const float qy = rot[i*4+1];
				const float qz = rot[i*4+2];
				const float qw = rot[i*4+3];
				S.Rotation = FQuat(qx, qy, qz, qw).GetNormalized();
			}

			if (Has(fdc, (i+1)*3))
			{
				S.Color = FLinearColor(fdc[i*3+0], fdc[i*3+1], fdc[i*3+2], 1.0f);
			}
			else if (Has(rgb, (i+1)*3))
			{
				// If stored as 0..255
				S.Color = FLinearColor(rgb[i*3+0]/255.0f, rgb[i*3+1]/255.0f, rgb[i*3+2]/255.0f, 1.0f);
			}

			if (Has(opacity, i+1))
			{
				S.Opacity = opacity[i];
			}

			OutSplats.Add(S);
		}

		return true;
	}
	catch (const std::exception& e)
	{
		OutError = FString::Printf(TEXT("tinyply exception: %s"), UTF8_TO_TCHAR(e.what()));
		return false;
	}
	catch (...)
	{
		OutError = TEXT("Unknown exception while reading PLY.");
		return false;
	}
}
