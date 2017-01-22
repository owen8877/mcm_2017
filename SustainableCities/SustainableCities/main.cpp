#include <iostream>
#include <cmath>
#include <algorithm>
#include <queue>
#include <string>
#include <fstream>
#include <iomanip>

#define MAX_ROW_SIZE 65
#define MAX_COL_SIZE 65
#define NEIGHBOURHOOD_SIZE 2
#define BUSCOVER_SIZE 1
#define MAX_LEVEL 4
#define RESIDENCE_DEVELOP 100
#define COMMERCE_DEVELOP 100
#define ENTERTAIN_DEVELOP 100
#define OPENSPACE_DEVELOP 100
#define BUSSTATION_DEVELOP 25
#define UNUSED_DEVELOP 20
#define NOTTINGHAM_INCREASE_RATE 1.137
#define BARRIE_INCREASE_RATE 1.0567

using namespace std;

namespace enumExt {
	template<typename T>
	inline T operator |=(T &a, const T &b)
	{
		return a = static_cast<T>(static_cast<int>(a) | static_cast<int>(b));
	}
	template<typename T>
	inline T operator |(const T &a, const T &b)
	{
		return static_cast<T>(static_cast<int>(a) | static_cast<int>(b));
	}
	template<typename T>
	inline T operator &=(T &a, const T &b)
	{
		return a = static_cast<T>(static_cast<int>(a) & static_cast<int>(b));
	}
	template<typename T>
	inline T operator &(const T &a, const T &b)
	{
		return static_cast<T>(static_cast<int>(a) & static_cast<int>(b));
	}
	template<typename T>
	inline T operator ++(T &a)
	{
		return a = static_cast<T>(static_cast<int>(a) + 1);
	}
	template<typename T>
	inline T operator ++(T &a, int k)
	{
		return a = static_cast<T>(static_cast<int>(a) + 1);
	}
	template<typename T>
	inline T operator ~(const T &a)
	{
		return static_cast<T>(~static_cast<int>(a));
	}
}

namespace area {
	enum areaType {
		residence = 0,
		commerce = 1,
		entertain = 2,
		openspace = 3,
		unused = 4,
		wasteland = 5,
	};
}

struct areaBlock {
	area::areaType type;
	int development;
	double population;
	bool busStation;
	areaBlock() : type(area::wasteland), development(0), busStation(false), population(0) { ; }
};

const char outFile[] = "output.coutput";
//const char inputFile[] = "Nottingham.cinput";
const char inputFile[] = "barrie.cinput";
//const char inputFile[] = "nottingham-patch.cinput";
//const char inputFile[] = "barrie-patch.cinput";
const char *outputFile = outFile;
const double affectParameter[5][5] = { {0, 0.5, 0.3, 0.2, 0}, 
									   {0.4, 0, 0.4, 0.2, 0}, 
									   {0.6, 0.3, 0, 0.1, 0}, 
									   {0.8, 0.1, 0.1, 0}, 
									   {0, 0, 0, 0, 0} };

areaBlock city[MAX_ROW_SIZE][MAX_COL_SIZE];
areaBlock originalCity[MAX_ROW_SIZE][MAX_COL_SIZE];
int population, popCapacity, blockCount[4];
double livingScore, openspaceScore, housingScore, transportScore, totalScore, rawLivingScore, rawOpenspaceScore;
double blockScore[MAX_ROW_SIZE][MAX_COL_SIZE];
double beautyScore[MAX_ROW_SIZE][MAX_COL_SIZE];
double fitScore[MAX_ROW_SIZE][MAX_COL_SIZE];

double adjust1(double x) {
	return (697.216*x - 352.132) / (6.97216*x + 1);
}

double adjust2(double x) {
	return (422.932*x - 160) / (4.22932*x - 1);
}

double _adjust1(double x) {
	return (3152.3366) / ((6.97216*x + 1) * (6.97216*x + 1));
}

double _adjust2(double x) {
	return (253.7592) / ((3.61736*x - 1)*(3.61736*x - 1));
}

void inputData() { 
	ifstream ifs(inputFile);
	while (!ifs.eof()) {
		int x, y, development, type;
		char busStation;
		ifs >> x >> y >> development >> type >> busStation;
		originalCity[x][y].population = city[x][y].population = development;
		originalCity[x][y].development = city[x][y].development = development;
		originalCity[x][y].type = city[x][y].type = static_cast<area::areaType>(type);
		originalCity[x][y].busStation = city[x][y].busStation = (busStation == 'T');
	}
}

void getUpgrade() {
	ifstream ifs("barrie.cinput");
	ifstream ifs2("barrie-patch.cinput");
	int temp1 = 0, temp2 = 0;
	double rate = 0;
	while (!ifs.eof()) {
		int x, y, development, type;
		char busStation;
		ifs >> x >> y >> development >> type >> busStation;
		originalCity[x][y].population = development;
		originalCity[x][y].development  = development;
		originalCity[x][y].type  = static_cast<area::areaType>(type);
		originalCity[x][y].busStation  = (busStation == 'T');
		temp2 += development;
		if (type == area::residence) temp1 += development;
	}
	while (!ifs2.eof()) {
		int x, y, development, type;
		char busStation;
		ifs2 >> x >> y >> development >> type >> busStation;
		city[x][y].population = development;
		city[x][y].development = development;
		city[x][y].type = static_cast<area::areaType>(type);
		city[x][y].busStation = (busStation == 'T');
	}
	int ans = 0;
	for (int x = 0; x < MAX_ROW_SIZE; x++)
		for (int y = 0; y < MAX_COL_SIZE; y++) ans += city[x][y].development - originalCity[x][y].development;
	rate = temp1 / static_cast<double>(temp2);
	cout << ans << endl;
	cout << ans / (1 - rate) << endl;
	cout << rate << endl;
}

void outputData() {
	ofstream ofs(outputFile);
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) {
			if (city[i][j].type != area::wasteland) ofs << i << ' ' << j << ' ' << city[i][j].development << ' ' << static_cast<int>(city[i][j].type) << ' '; else continue;
			if (city[i][j].busStation) ofs << 'T' << endl; else ofs << 'F' << endl;
		}
}

void outputDiff() {
	ofstream ofs("diff.coutput");
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) if (originalCity[i][j].development < city[i][j].development) ofs << i << ' ' << j << ' ' << city[i][j].development - originalCity[i][j].development << ' ' << static_cast<int>(city[i][j].type) << endl;
}

void getBlockInfo() {
	using namespace enumExt;
	for (area::areaType i = area::residence; i < area::unused; i++) blockCount[i] = 0;
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) {
			double temp = 0;
			for (int k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
				for (int l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) temp += city[k][l].development / static_cast<double>(abs(k - i)*abs(k - i) + abs(l - j)*abs(l - j) + 1) * affectParameter[city[i][j].type][city[k][l].type];
			blockScore[i][j] = temp * sqrt(city[i][j].development);
			if (city[i][j].type < area::unused) blockCount[city[i][j].type]++;
		}
}

void getBeautyInfo() {
	using namespace enumExt;
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) {
			if (city[i][j].type > area::openspace) {
				beautyScore[i][j] = 0;
				continue;
			}
			double temp = 0;
			for (int k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
				for (int l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::openspace) temp += city[k][l].development / static_cast<double>(abs(k - i)*abs(k - i) + abs(l - j)*abs(l - j) + 1);
			beautyScore[i][j] = temp;
		}
}

void getLivingScore() { 
	livingScore = 0;
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) if (city[i][j].type == area::residence) livingScore += blockScore[i][j];
	livingScore /= static_cast<double>(blockCount[area::residence]);
	rawLivingScore = livingScore;
	livingScore = adjust1(livingScore);
}

void getOpenSpaceScore() {
	using namespace enumExt;
	openspaceScore = 0;
	double count = 0;
	for (area::areaType i = area::residence; i < area::openspace; i++) count += blockCount[i];
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) if (city[i][j].type < area::openspace) openspaceScore += beautyScore[i][j];
	openspaceScore /= count;
	rawOpenspaceScore = openspaceScore;
	openspaceScore = adjust2(openspaceScore);
}

double getFitScore(int i, int j) {
	int temp = city[i][j].development;
	for (int k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
		for (int l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::residence) temp = min(temp, abs(city[k][l].development - city[i][j].development));
	return 100 * (1 - temp / static_cast<double>(city[i][j].development));
}

void getHousingScore() {
	using namespace enumExt;
	housingScore = 0;
	double count = blockCount[area::commerce] + blockCount[area::entertain];
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) if (city[i][j].type == area::commerce || city[i][j].type == area::entertain) {
			fitScore[i][j] = getFitScore(i, j);
			housingScore += fitScore[i][j];
		}
	housingScore /= count;
}

void getTransportScore() {
	using namespace enumExt;
	bool blockUsed[MAX_ROW_SIZE][MAX_COL_SIZE];
	transportScore = 0;
	double count = 0;
	for (area::areaType i = area::residence; i < area::unused; i++) count += blockCount[i];
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) blockUsed[i][j] = false;
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) if (city[i][j].busStation)
			for (int k = max(0, i - BUSCOVER_SIZE); k <= min(MAX_ROW_SIZE - 1, i + BUSCOVER_SIZE); k++)
				for (int l = max(0, j - BUSCOVER_SIZE); l <= min(MAX_COL_SIZE - 1, j + BUSCOVER_SIZE); l++) blockUsed[k][l] = true;
	int coverNum = 0;
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) if (blockUsed[i][j] && city[i][j].type < area::unused) coverNum++;
	transportScore = coverNum / count * 100;
}

void upgradeResidence(int _limit) {
	bool blockUsed[MAX_ROW_SIZE][MAX_COL_SIZE];
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) blockUsed[i][j] = false;
	while (_limit > 0) {
		double maxIncrease = 0;
		int x, y, develop;
		for (int i = 0; i < MAX_ROW_SIZE; i++)
			for (int j = 0; j < MAX_COL_SIZE; j++) {
				if (blockUsed[i][j] || city[i][j].type != area::residence) continue;
				for (int add = 1; add + city[i][j].development <= MAX_LEVEL && add <= _limit; add++) {
					city[i][j].development += add;
					double temp1 = 0, temp2 = 0;
					temp1 += blockScore[i][j] * (sqrt(city[i][j].development) / sqrt(city[i][j].development - add) - 1) / blockCount[area::residence] * _adjust1(rawLivingScore); // livingScore Change
					temp1 += 0; // openspaceScore Change
					temp2 = 0;
					for (int k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
						for (int l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::commerce || city[k][l].type == area::entertain) temp2 += getFitScore(k, l) - fitScore[k][l];
					temp1 += temp2 / static_cast<double>(blockCount[area::commerce] + blockCount[area::entertain]); // housingScore Change
					temp1 /= static_cast<double>(city[i][j].development);
					if (temp1 / static_cast<double>(add) > maxIncrease) {
						maxIncrease = temp1 / static_cast<double>(add);
						x = i;
						y = j;
						develop = add;
					}
					city[i][j].development -= add;
				}
			}
		if (maxIncrease == 0) break;
		city[x][y].development += develop;
		blockUsed[x][y] = true;
		_limit -= develop;
		getBlockInfo();
		getBeautyInfo();
		getLivingScore();
		getOpenSpaceScore();
		getHousingScore();
		getTransportScore();
	}
}

void upgradeCommerce(int _limit) {
	bool blockUsed[MAX_ROW_SIZE][MAX_COL_SIZE];
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) blockUsed[i][j] = false;
	while (_limit > 0) {
		double maxIncrease = 0;
		int x, y, develop;
		for (int i = 0; i < MAX_ROW_SIZE; i++)
			for (int j = 0; j < MAX_COL_SIZE; j++) {
				if (blockUsed[i][j] || city[i][j].type != area::commerce) continue;
				for (int add = 1; add + city[i][j].development <= MAX_LEVEL && add <= _limit; add++) {
					city[i][j].development += add;
					double temp1 = 0, temp2 = 0;
					for (int k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
						for (int l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::residence) temp2 += add * sqrt(city[k][l].development) * affectParameter[area::residence][area::commerce] / static_cast<double>(abs(k - i) * abs(k - i) + abs(l - j)*abs(l - j) + 1);
					temp1 += temp2 / blockCount[area::residence] * _adjust1(rawLivingScore); // livingScore Change
					temp1 += 0; // openspaceScore Change
					temp2 = 0;
					for (int k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
						for (int l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::commerce || city[k][l].type == area::entertain) temp2 += getFitScore(k, l) - fitScore[k][l];
					temp1 += temp2 / static_cast<double>(blockCount[area::commerce] + blockCount[area::entertain]); // housingScore Change
					temp1 /= static_cast<double>(city[i][j].development);
					if (temp1 / static_cast<double>(add) > maxIncrease) {
						maxIncrease = temp1 / static_cast<double>(add);
						x = i;
						y = j;
						develop = add;
					}
					city[i][j].development -= add;
				}
			}
		if (maxIncrease == 0) break;
		city[x][y].development += develop;
		blockUsed[x][y] = true;
		_limit -= develop;
		getBlockInfo();
		getBeautyInfo();
		getLivingScore();
		getOpenSpaceScore();
		getHousingScore();
		getTransportScore();
	}
}

void upgradeEntertain(int _limit) {
	bool blockUsed[MAX_ROW_SIZE][MAX_COL_SIZE];
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) blockUsed[i][j] = false;
	while (_limit > 0) {
		double maxIncrease = 0;
		int x, y, develop;
		for (int i = 0; i < MAX_ROW_SIZE; i++)
			for (int j = 0; j < MAX_COL_SIZE; j++) {
				if (blockUsed[i][j] || city[i][j].type != area::entertain) continue;
				for (int add = 1; add + city[i][j].development <= MAX_LEVEL && add <= _limit; add++) {
					city[i][j].development += add;
					double temp1 = 0, temp2 = 0;
					for (int k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
						for (int l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::residence) temp2 += add * sqrt(city[k][l].development) * affectParameter[area::residence][area::entertain] / static_cast<double>(abs(k - i) * abs(k - i) + abs(l - j)*abs(l - j) + 1);
					temp1 += temp2 / blockCount[area::residence] * _adjust1(rawLivingScore); // livingScore Change
					temp1 += 0; // openspaceScore Change
					temp2 = 0;
					for (int k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
						for (int l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::commerce || city[k][l].type == area::entertain) temp2 += getFitScore(k, l) - fitScore[k][l];
					temp1 += temp2 / static_cast<double>(blockCount[area::commerce] + blockCount[area::entertain]); // housingScore Change
					temp1 /= static_cast<double>(city[i][j].development);
					if (temp1 / static_cast<double>(add) > maxIncrease) {
						maxIncrease = temp1 / static_cast<double>(add);
						x = i;
						y = j;
						develop = add;
					}
					city[i][j].development -= add;
				}
			}
		if (maxIncrease == 0) break;
		city[x][y].development += develop;
		blockUsed[x][y] = true;
		_limit -= develop;
		getBlockInfo();
		getBeautyInfo();
		getLivingScore();
		getOpenSpaceScore();
		getHousingScore();
		getTransportScore();
	}
}

void upgradeOpenspace(int _limit) {
	bool blockUsed[MAX_ROW_SIZE][MAX_COL_SIZE];
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) blockUsed[i][j] = false;
	while (_limit > 0) {
		double maxIncrease = 0;
		int x, y, develop;
		for (int i = 0; i < MAX_ROW_SIZE; i++)
			for (int j = 0; j < MAX_COL_SIZE; j++) {
				if (blockUsed[i][j] || city[i][j].type != area::openspace) continue;
				for (int add = 1; add + city[i][j].development <= MAX_LEVEL && add <= _limit; add++) {
					city[i][j].development += add;
					double temp1 = 0, temp2 = 0;
					for (int k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
						for (int l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::residence) temp2 += add * sqrt(city[k][l].development) * affectParameter[area::residence][area::openspace] / static_cast<double>(abs(k - i) * abs(k - i) + abs(l - j)*abs(l - j) + 1);
					temp1 += temp2 / blockCount[area::residence] * _adjust1(rawLivingScore); // livingScore Change
					temp2 = 0;
					for (int k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
						for (int l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type < area::openspace) temp2 += add / static_cast<double>(abs(k - i) * abs(k - i) + abs(l - j)*abs(l - j) + 1);
					temp1 += temp2 / (blockCount[area::residence] + blockCount[area::commerce] + blockCount[area::entertain]) * _adjust2(rawOpenspaceScore); // openspaceScore Change
					temp1 += 0; // housingScore Change
					temp1 /= static_cast<double>(city[i][j].development);
					if (temp1 / static_cast<double>(add) > maxIncrease) {
						maxIncrease = temp1 / static_cast<double>(add);
						x = i;
						y = j;
						develop = add;
					}
					city[i][j].development -= add;
				}
			}
		if (maxIncrease == 0) break;
		city[x][y].development += develop;
		blockUsed[x][y] = true;
		_limit -= develop;
		getBlockInfo();
		getBeautyInfo();
		getLivingScore();
		getOpenSpaceScore();
		getHousingScore();
		getTransportScore();
	}
}

void upgradeUnused(int _limit) {
	using namespace enumExt;
	bool blockUsed[MAX_ROW_SIZE][MAX_COL_SIZE];
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) blockUsed[i][j] = false;
	while (_limit > 0) {
		double maxIncrease = 0;
		int x, y, develop;
		area::areaType developType;
		for (int i = 0; i < MAX_ROW_SIZE; i++)
			for (int j = 0; j < MAX_COL_SIZE; j++) {
				if (blockUsed[i][j] || (city[i][j].type != area::unused && city[i][j].type != area::openspace) || (city[i][j].type == area::openspace && city[i][j].development > 1)) continue;
				if (city[i][j].type == area::unused) {
					for (area::areaType curType = area::residence; curType < area::unused; curType++) {
						for (int add = 0; add <= MAX_LEVEL && add <= _limit; add++) {
							city[i][j].development = add;
							city[i][j].type = curType;
							double temp1 = 0, temp2 = 0;
							int k, l;
							switch (curType) {
							case area::residence:
								temp1 += blockScore[i][j] * sqrt(city[i][j].development) / blockCount[area::residence] * _adjust1(rawLivingScore); // livingScore Change
								temp1 += 0; // openspaceScore Change
								for (k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
									for (l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::commerce || city[k][l].type == area::entertain) temp2 += getFitScore(k, l) - fitScore[k][l];
								temp2 = 0;
								temp1 += temp2 / static_cast<double>(blockCount[area::commerce] + blockCount[area::entertain]); // housingScore Change
								break;
							case area::commerce:
								for (k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
									for (l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::residence) temp2 += add * sqrt(city[k][l].development) * affectParameter[area::residence][area::commerce] / static_cast<double>(abs(k - i) * abs(k - i) + abs(l - j)*abs(l - j) + 1);
								temp1 += temp2 / blockCount[area::residence] * _adjust1(rawLivingScore); // livingScore Change
								temp1 += 0; // openspaceScore Change
								for (k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
									for (l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::commerce || city[k][l].type == area::entertain) temp2 += getFitScore(k, l) - fitScore[k][l];
								temp2 = 0;
								temp1 += temp2 / static_cast<double>(blockCount[area::commerce] + blockCount[area::entertain]); // housingScore Change
								break;
							case area::entertain:
								for (k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
									for (l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::residence) temp2 += add * sqrt(city[k][l].development) * affectParameter[area::residence][area::entertain] / static_cast<double>(abs(k - i) * abs(k - i) + abs(l - j)*abs(l - j) + 1);
								temp1 += temp2 / blockCount[area::residence] * _adjust1(rawLivingScore); // livingScore Change
								temp1 += 0; // openspaceScore Change
								for (k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
									for (l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::commerce || city[k][l].type == area::entertain) temp2 += getFitScore(k, l) - fitScore[k][l];
								temp2 = 0;
								temp1 += temp2 / static_cast<double>(blockCount[area::commerce] + blockCount[area::entertain]); // housingScore Change
								break;
							case area::openspace:
								for (k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
									for (l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::residence) temp2 += add * sqrt(city[k][l].development) * affectParameter[area::residence][area::openspace] / static_cast<double>(abs(k - i) * abs(k - i) + abs(l - j)*abs(l - j) + 1);
								temp1 += temp2 / blockCount[area::residence] * _adjust1(rawLivingScore); // livingScore Change
								temp2 = 0;
								for (k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
									for (l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type < area::openspace) temp2 += add / static_cast<double>(abs(k - i) * abs(k - i) + abs(l - j)*abs(l - j) + 1);
								temp1 += temp2 / (blockCount[area::residence] + blockCount[area::commerce] + blockCount[area::entertain]) * _adjust2(rawOpenspaceScore); // openspaceScore Change
								temp1 += 0; // housingScore Change
								break;
							}
							if (temp1 / static_cast<double>(add) > maxIncrease) {
								maxIncrease = temp1 / static_cast<double>(add);
								x = i;
								y = j;
								develop = add;
								developType = curType;
							}
							city[i][j].development = 0;
							city[i][j].type = area::unused;
						}
					}
				}
				else {
					for (area::areaType curType = area::residence; curType < area::unused; curType++) {
						if (curType == area::openspace) continue;
						for (int add = 0; add <= MAX_LEVEL && add <= _limit; add++) {
							city[i][j].development = add;
							city[i][j].type = curType;
							double temp1 = 0, temp2 = 0;
							int k, l;
							switch (curType) {
							case area::residence:
								temp1 += blockScore[i][j] * sqrt(city[i][j].development) / blockCount[area::residence] * _adjust1(rawLivingScore); // livingScore Change
								temp2 = 0;
								for (k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
									for (l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type < area::openspace) temp2 -= 1 / static_cast<double>(abs(k - i) * abs(k - i) + abs(l - j)*abs(l - j) + 1);
								temp1 += temp2 / (blockCount[area::residence] + blockCount[area::commerce] + blockCount[area::entertain]) * _adjust2(rawOpenspaceScore); // openspaceScore Change
								for (k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
									for (l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::commerce || city[k][l].type == area::entertain) temp2 += getFitScore(k, l) - fitScore[k][l];
								temp2 = 0;
								temp1 += temp2 / static_cast<double>(blockCount[area::commerce] + blockCount[area::entertain]); // housingScore Change
								break;
							case area::commerce:
								for (k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
									for (l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::residence) temp2 += add * sqrt(city[k][l].development) * affectParameter[area::residence][area::commerce] / static_cast<double>(abs(k - i) * abs(k - i) + abs(l - j)*abs(l - j) + 1);
								temp1 += temp2 / blockCount[area::residence] * _adjust1(rawLivingScore); // livingScore Change
								temp2 = 0;
								for (k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
									for (l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type < area::openspace) temp2 -= 1 / static_cast<double>(abs(k - i) * abs(k - i) + abs(l - j)*abs(l - j) + 1);
								temp1 += temp2 / (blockCount[area::residence] + blockCount[area::commerce] + blockCount[area::entertain]) * _adjust2(rawOpenspaceScore); // openspaceScore Change
								for (k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
									for (l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::commerce || city[k][l].type == area::entertain) temp2 += getFitScore(k, l) - fitScore[k][l];
								temp2 = 0;
								temp1 += temp2 / static_cast<double>(blockCount[area::commerce] + blockCount[area::entertain]); // housingScore Change
								break;
							case area::entertain:
								for (k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
									for (l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::residence) temp2 += add * sqrt(city[k][l].development) * affectParameter[area::residence][area::entertain] / static_cast<double>(abs(k - i) * abs(k - i) + abs(l - j)*abs(l - j) + 1);
								temp1 += temp2 / blockCount[area::residence] * _adjust1(rawLivingScore); // livingScore Change
								temp2 = 0;
								for (k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
									for (l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type < area::openspace) temp2 -= 1 / static_cast<double>(abs(k - i) * abs(k - i) + abs(l - j)*abs(l - j) + 1);
								temp1 += temp2 / (blockCount[area::residence] + blockCount[area::commerce] + blockCount[area::entertain]) * _adjust2(rawOpenspaceScore); // openspaceScore Change
								for (k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
									for (l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::commerce || city[k][l].type == area::entertain) temp2 += getFitScore(k, l) - fitScore[k][l];
								temp2 = 0;
								temp1 += temp2 / static_cast<double>(blockCount[area::commerce] + blockCount[area::entertain]); // housingScore Change
								break;
								if (temp1 / static_cast<double>(add) > maxIncrease) {
									maxIncrease = temp1 / static_cast<double>(add);
									x = i;
									y = j;
									develop = add;
									developType = curType;
								}
								city[i][j].development = 0;
								city[i][j].type = area::unused;
							}
						}
					}
				}
			}
		if (maxIncrease == 0) break;
		city[x][y].development = develop;
		city[x][y].type = developType;
		blockUsed[x][y] = true;
		_limit -= develop;
		getBlockInfo();
		getBeautyInfo();
		getLivingScore();
		getOpenSpaceScore();
		getHousingScore();
		getTransportScore();
	}
}

void upgradeBusStation(int _limit) {
	using namespace enumExt;
	bool blockUsed[MAX_ROW_SIZE][MAX_COL_SIZE];
	int blockNum = 0, coverNum;
	for (area::areaType i = area::residence; i < area::unused; i++) blockNum += blockCount[i];
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) blockUsed[i][j] = false;
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) if (city[i][j].busStation)
			for (int k = max(0, i - BUSCOVER_SIZE); k <= min(MAX_ROW_SIZE - 1, i + BUSCOVER_SIZE); k++)
				for (int l = max(0, j - BUSCOVER_SIZE); l <= min(MAX_COL_SIZE - 1, j + BUSCOVER_SIZE); l++) blockUsed[k][l] = true;
	coverNum = 0;
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) if (blockUsed[i][j] && city[i][j].type < area::unused) coverNum++;
	while (coverNum < blockNum && (_limit--)) {
		int maxIncrease = 0, x, y;
		for (int i = 0; i < MAX_ROW_SIZE; i++)
			for (int j = 0; j < MAX_COL_SIZE; j++) {
				int temp = 0;
				if (city[i][j].type > area::openspace || city[i][j].busStation) continue;
				for (int k = max(0, i - BUSCOVER_SIZE); k <= min(MAX_ROW_SIZE - 1, i + BUSCOVER_SIZE); k++)
					for (int l = max(0, j - BUSCOVER_SIZE); l <= min(MAX_COL_SIZE - 1, j + BUSCOVER_SIZE); l++) if (city[k][l].type < area::unused && !blockUsed[k][l]) temp++;
				if (temp > maxIncrease) {
					maxIncrease = temp;
					x = i;
					y = j;
				}
			}
		city[x][y].busStation = true;
		for (int i = 0; i < MAX_ROW_SIZE; i++)
			for (int j = 0; j < MAX_COL_SIZE; j++) blockUsed[i][j] = false;
		for (int i = 0; i < MAX_ROW_SIZE; i++)
			for (int j = 0; j < MAX_COL_SIZE; j++) if (city[i][j].busStation)
				for (int k = max(0, i - BUSCOVER_SIZE); k <= min(MAX_ROW_SIZE - 1, i + BUSCOVER_SIZE); k++)
					for (int l = max(0, j - BUSCOVER_SIZE); l <= min(MAX_COL_SIZE - 1, j + BUSCOVER_SIZE); l++) blockUsed[k][l] = true;
		coverNum = 0;
		for (int i = 0; i < MAX_ROW_SIZE; i++)
			for (int j = 0; j < MAX_COL_SIZE; j++) if (blockUsed[i][j] && city[i][j].type < area::unused) coverNum++;
	}
	getTransportScore();
}

void printScore() {
	totalScore = (livingScore + openspaceScore + housingScore + transportScore) / static_cast<double>(4);
	cout << livingScore << endl;
	cout << openspaceScore << endl;
	cout << housingScore << endl;
	cout << transportScore << endl;
	cout << totalScore << endl;
	cout << endl;
}

int main() {	
	cout << setiosflags(ios::fixed) << setprecision(4);
	//inputData();
	//getBlockInfo();
	//getBeautyInfo();
	//getLivingScore();
	//getOpenSpaceScore();
	//getHousingScore();
	//getTransportScore();
	//printScore();
	//upgradeResidence(RESIDENCE_DEVELOP);
	//upgradeCommerce(COMMERCE_DEVELOP);
	//upgradeEntertain(ENTERTAIN_DEVELOP);
	//upgradeOpenspace(OPENSPACE_DEVELOP);
	//upgradeUnused(UNUSED_DEVELOP);
	//upgradeBusStation(BUSSTATION_DEVELOP);
	//printScore();
	//outputData();
	//outputDiff();
	getUpgrade();
	system("pause");
}