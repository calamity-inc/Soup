#include "loc.hpp"

#include "deflate.hpp"
#include "joaat.hpp"
#include "string.hpp"
#include "StringReader.hpp"

#include "loc_data.hpp"

NAMESPACE_SOUP
{
	std::unordered_map<uint32_t, std::string> loc::getDict(std::string lang_code)
	{
		string::replaceAll(lang_code, "-", "_");
		string::lower(lang_code);
		switch (joaat::hash(lang_code))
		{
		case joaat::compileTimeHash("af"): return loadDict(compressed_af, sizeof(compressed_af));
		case joaat::compileTimeHash("am"): return loadDict(compressed_am, sizeof(compressed_am));
		case joaat::compileTimeHash("ar"): return loadDict(compressed_ar, sizeof(compressed_ar));
		case joaat::compileTimeHash("az"): return loadDict(compressed_az, sizeof(compressed_az));
		case joaat::compileTimeHash("be"): return loadDict(compressed_be, sizeof(compressed_be));
		case joaat::compileTimeHash("bg"): return loadDict(compressed_bg, sizeof(compressed_bg));
		case joaat::compileTimeHash("bn"): return loadDict(compressed_bn, sizeof(compressed_bn));
		case joaat::compileTimeHash("bo"): return loadDict(compressed_bo, sizeof(compressed_bo));
		case joaat::compileTimeHash("ca"): return loadDict(compressed_ca, sizeof(compressed_ca));
		case joaat::compileTimeHash("cs"): return loadDict(compressed_cs, sizeof(compressed_cs));
		case joaat::compileTimeHash("cy"): return loadDict(compressed_cy, sizeof(compressed_cy));
		case joaat::compileTimeHash("da"): return loadDict(compressed_da, sizeof(compressed_da));
		case joaat::compileTimeHash("de"): return loadDict(compressed_de, sizeof(compressed_de));
		case joaat::compileTimeHash("ee"): return loadDict(compressed_ee, sizeof(compressed_ee));
		case joaat::compileTimeHash("el"): return loadDict(compressed_el, sizeof(compressed_el));
		case joaat::compileTimeHash("en"): return loadDict(compressed_en, sizeof(compressed_en));
		case joaat::compileTimeHash("eo"): return loadDict(compressed_eo, sizeof(compressed_eo));
		case joaat::compileTimeHash("es"): return loadDict(compressed_es, sizeof(compressed_es));
		case joaat::compileTimeHash("et"): return loadDict(compressed_et, sizeof(compressed_et));
		case joaat::compileTimeHash("eu"): return loadDict(compressed_eu, sizeof(compressed_eu));
		case joaat::compileTimeHash("fa"): return loadDict(compressed_fa, sizeof(compressed_fa));
		case joaat::compileTimeHash("fi"): return loadDict(compressed_fi, sizeof(compressed_fi));
		case joaat::compileTimeHash("fo"): return loadDict(compressed_fo, sizeof(compressed_fo));
		case joaat::compileTimeHash("fr"): return loadDict(compressed_fr, sizeof(compressed_fr));
		case joaat::compileTimeHash("ga"): return loadDict(compressed_ga, sizeof(compressed_ga));
		case joaat::compileTimeHash("gl"): return loadDict(compressed_gl, sizeof(compressed_gl));
		case joaat::compileTimeHash("gu"): return loadDict(compressed_gu, sizeof(compressed_gu));
		case joaat::compileTimeHash("he"): return loadDict(compressed_he, sizeof(compressed_he));
		case joaat::compileTimeHash("hi"): return loadDict(compressed_hi, sizeof(compressed_hi));
		case joaat::compileTimeHash("hr"): return loadDict(compressed_hr, sizeof(compressed_hr));
		case joaat::compileTimeHash("hu"): return loadDict(compressed_hu, sizeof(compressed_hu));
		case joaat::compileTimeHash("hy"): return loadDict(compressed_hy, sizeof(compressed_hy));
		case joaat::compileTimeHash("ia"): return loadDict(compressed_ia, sizeof(compressed_ia));
		case joaat::compileTimeHash("id"): return loadDict(compressed_id, sizeof(compressed_id));
		case joaat::compileTimeHash("is"): return loadDict(compressed_is, sizeof(compressed_is));
		case joaat::compileTimeHash("it"): return loadDict(compressed_it, sizeof(compressed_it));
		case joaat::compileTimeHash("ja"): return loadDict(compressed_ja, sizeof(compressed_ja));
		case joaat::compileTimeHash("ka"): return loadDict(compressed_ka, sizeof(compressed_ka));
		case joaat::compileTimeHash("ki"): return loadDict(compressed_ki, sizeof(compressed_ki));
		case joaat::compileTimeHash("km"): return loadDict(compressed_km, sizeof(compressed_km));
		case joaat::compileTimeHash("kn"): return loadDict(compressed_kn, sizeof(compressed_kn));
		case joaat::compileTimeHash("ko"): return loadDict(compressed_ko, sizeof(compressed_ko));
		case joaat::compileTimeHash("lg"): return loadDict(compressed_lg, sizeof(compressed_lg));
		case joaat::compileTimeHash("lo"): return loadDict(compressed_lo, sizeof(compressed_lo));
		case joaat::compileTimeHash("lt"): return loadDict(compressed_lt, sizeof(compressed_lt));
		case joaat::compileTimeHash("lv"): return loadDict(compressed_lv, sizeof(compressed_lv));
		case joaat::compileTimeHash("mk"): return loadDict(compressed_mk, sizeof(compressed_mk));
		case joaat::compileTimeHash("ml"): return loadDict(compressed_ml, sizeof(compressed_ml));
		case joaat::compileTimeHash("mr"): return loadDict(compressed_mr, sizeof(compressed_mr));
		case joaat::compileTimeHash("ms"): return loadDict(compressed_ms, sizeof(compressed_ms));
		case joaat::compileTimeHash("mt"): return loadDict(compressed_mt, sizeof(compressed_mt));
		case joaat::compileTimeHash("my"): return loadDict(compressed_my, sizeof(compressed_my));
		case joaat::compileTimeHash("nb"): return loadDict(compressed_nb, sizeof(compressed_nb));
		case joaat::compileTimeHash("ne"): return loadDict(compressed_ne, sizeof(compressed_ne));
		case joaat::compileTimeHash("nl"): return loadDict(compressed_nl, sizeof(compressed_nl));
		case joaat::compileTimeHash("nn"): return loadDict(compressed_nn, sizeof(compressed_nn));
		case joaat::compileTimeHash("no"): return loadDict(compressed_no, sizeof(compressed_no));
		case joaat::compileTimeHash("or"): return loadDict(compressed_or, sizeof(compressed_or));
		case joaat::compileTimeHash("pl"): return loadDict(compressed_pl, sizeof(compressed_pl));
		case joaat::compileTimeHash("pt"): return loadDict(compressed_pt, sizeof(compressed_pt));
		case joaat::compileTimeHash("rm"): return loadDict(compressed_rm, sizeof(compressed_rm));
		case joaat::compileTimeHash("ro"): return loadDict(compressed_ro, sizeof(compressed_ro));
		case joaat::compileTimeHash("ru"): return loadDict(compressed_ru, sizeof(compressed_ru));
		case joaat::compileTimeHash("se"): return loadDict(compressed_se, sizeof(compressed_se));
		case joaat::compileTimeHash("sk"): return loadDict(compressed_sk, sizeof(compressed_sk));
		case joaat::compileTimeHash("sl"): return loadDict(compressed_sl, sizeof(compressed_sl));
		case joaat::compileTimeHash("sn"): return loadDict(compressed_sn, sizeof(compressed_sn));
		case joaat::compileTimeHash("sq"): return loadDict(compressed_sq, sizeof(compressed_sq));
		case joaat::compileTimeHash("sr"): return loadDict(compressed_sr, sizeof(compressed_sr));
		case joaat::compileTimeHash("sv"): return loadDict(compressed_sv, sizeof(compressed_sv));
		case joaat::compileTimeHash("ta"): return loadDict(compressed_ta, sizeof(compressed_ta));
		case joaat::compileTimeHash("te"): return loadDict(compressed_te, sizeof(compressed_te));
		case joaat::compileTimeHash("th"): return loadDict(compressed_th, sizeof(compressed_th));
		case joaat::compileTimeHash("ti"): return loadDict(compressed_ti, sizeof(compressed_ti));
		case joaat::compileTimeHash("tl"): return loadDict(compressed_tl, sizeof(compressed_tl));
		case joaat::compileTimeHash("tr"): return loadDict(compressed_tr, sizeof(compressed_tr));
		case joaat::compileTimeHash("uk"): return loadDict(compressed_uk, sizeof(compressed_uk));
		case joaat::compileTimeHash("ur"): return loadDict(compressed_ur, sizeof(compressed_ur));
		case joaat::compileTimeHash("vi"): return loadDict(compressed_vi, sizeof(compressed_vi));
		case joaat::compileTimeHash("zh_cn"): return loadDict(compressed_zh_CN, sizeof(compressed_zh_CN));
		case joaat::compileTimeHash("zh_tw"): return loadDict(compressed_zh_TW, sizeof(compressed_zh_TW));
		}
		return {};
	}

	std::unordered_map<uint32_t,std::string> loc::loadDict(const char* data, size_t size)
	{
		std::unordered_map<uint32_t,std::string> dict;
		StringReader sr(deflate::decompress(data, size).decompressed);
		while (sr.hasMore())
		{
			uint32_t key;
			sr.u32_le(key);
			std::string value;
			sr.str_nt(value);
			dict.emplace(key, std::move(value));
		}
		return dict;
	}
}
