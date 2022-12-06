#include "audDevice.hpp"

#if SOUP_WINDOWS

#include <Windows.h>
#include <mmeapi.h>
#include <objbase.h>
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>

#include "audPlayback.hpp"
#include "unicode.hpp"

namespace soup
{
	audDevice audDevice::get(int i)
	{
		WAVEOUTCAPSW woc;
		SOUP_IF_UNLIKELY (waveOutGetDevCapsW(i, &woc, sizeof(WAVEOUTCAPSW)) != S_OK)
		{
			throw 0;
		}
		return audDevice{
			i,
			woc.szPname
		};
	}

	std::vector<audDevice> audDevice::getAll()
	{
		std::vector<audDevice> res;
		int num = waveOutGetNumDevs();
		for (int i = 0; i != num; ++i)
		{
			res.emplace_back(get(i));
		}
		return res;
	}

	[[nodiscard]] static std::vector<std::wstring> getFullDeviceNamesInWaveOutOrder()
	{
		std::vector<std::wstring> res;

		HRESULT hr = CoInitialize(NULL);

		if (SUCCEEDED(hr))
		{
			IMMDeviceEnumerator* pEnum = NULL;

			hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum);

			if (SUCCEEDED(hr))
			{
				LPWSTR defaultDeviceId = NULL;
				IMMDevice* pDefaultDevice;

				HRESULT hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultDevice);

				if (SUCCEEDED(hr))
				{
					hr = pDefaultDevice->GetId(&defaultDeviceId);

					if (!SUCCEEDED(hr))
					{
						defaultDeviceId = NULL;
					}

					pDefaultDevice->Release();
				}

				IMMDeviceCollection* pDevices;

				hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);

				if (SUCCEEDED(hr))
				{
					UINT count;

					pDevices->GetCount(&count);

					if (SUCCEEDED(hr))
					{
						for (UINT i = 0; i != count; ++i)
						{
							IMMDevice* pDevice;
							HRESULT hr = pDevices->Item(i, &pDevice);

							if (SUCCEEDED(hr))
							{
								LPWSTR wstrID = NULL;

								hr = pDevice->GetId(&wstrID);

								if (SUCCEEDED(hr))
								{
									IPropertyStore* pStore;
									hr = pDevice->OpenPropertyStore(STGM_READ, &pStore);

									if (SUCCEEDED(hr))
									{
										PROPVARIANT friendlyName;

										PropVariantInit(&friendlyName);
										hr = pStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);

										if (SUCCEEDED(hr))
										{
											if (wcscmp(wstrID, defaultDeviceId) == 0) // Default device?
											{
												res.emplace(res.begin(), friendlyName.pwszVal);
											}
											else
											{
												res.emplace_back(friendlyName.pwszVal);
											}

											PropVariantClear(&friendlyName);
										}

										pStore->Release();
									}
								}

								pDevice->Release();
							}
						}
					}

					pDevices->Release();
				}

				pEnum->Release();
			}
		}

		return res;
	}

	std::string audDevice::getName() const
	{
		if (name.length() == MAXPNAMELEN - 1) // Name got cucked by waveOut device name limit?
		{
			auto fullnames = getFullDeviceNamesInWaveOutOrder();
			// Fullnames _should_ be in the same order as waveOut devices, but let's be sure...
			if (i < fullnames.size()
				&& fullnames.at(i).substr(0, 31) == name
				)
			{
				return unicode::utf16_to_utf8(fullnames.at(i));
			}
		}
		return unicode::utf16_to_utf8(name);
	}

	UniquePtr<audPlayback> audDevice::open(audGetAmplitude src) const
	{
		return soup::make_unique<audPlayback>(*this, src);
	}
}

#endif
