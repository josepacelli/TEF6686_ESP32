#include "rds_capture.h"
#include <map>
#include <vector>

static std::map<uint32_t, std::vector<String>> capPS;
static std::map<uint32_t, std::vector<String>> capRT;

static void addTo(std::map<uint32_t, std::vector<String>>& m, uint32_t freq, const String& s) {
    if (s.length() == 0) return;
    String su = s; su.trim();
    if (su.length() == 0) return;
    auto& v = m[freq];
    for (auto& e : v) if (e == su) return;
    if ((int)v.size() >= RDS_CAPTURE_MAX) v.erase(v.begin());
    v.push_back(su);
}

void addCapturedPS(uint32_t freq_khz, const String& s) { addTo(capPS, freq_khz, s); }
void addCapturedRT(uint32_t freq_khz, const String& s) { addTo(capRT, freq_khz, s); }

String getCapturedPS(uint32_t freq_khz, int i) {
    auto it = capPS.find(freq_khz);
    if (it == capPS.end() || i < 0 || i >= (int)it->second.size()) return "";
    return it->second[i];
}
String getCapturedRT(uint32_t freq_khz, int i) {
    auto it = capRT.find(freq_khz);
    if (it == capRT.end() || i < 0 || i >= (int)it->second.size()) return "";
    return it->second[i];
}
int getCapturedPSCount(uint32_t freq_khz) {
    auto it = capPS.find(freq_khz);
    return (it == capPS.end()) ? 0 : (int)it->second.size();
}
int getCapturedRTCount(uint32_t freq_khz) {
    auto it = capRT.find(freq_khz);
    return (it == capRT.end()) ? 0 : (int)it->second.size();
}
