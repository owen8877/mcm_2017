#include <iostream>
#include <cmath>
#include <algorithm>
#include <queue>

#define MAX_ROW_SIZE 100
#define MAX_COL_SIZE 100
#define NEIGHBOURHOOD_SIZE 5
#define BUSCOVER_SIZE 1
#define MAX_LEVEL 4

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
	bool busStation;
	areaBlock() : type(area::wasteland), development(0), busStation(false) { ; }
	areaBlock(area::areaType _type, int _development, bool _busStation = false) : type(_type), development(_development), busStation(_busStation) { ; }
};

const double overallScoreRatio[4] = { 25, 25, 25, 25 };
const double affectParameter[5][5] = { {0, 0.5, 0.25, 0.25, 0}, 
									   {0.4, 0, 0.4, 0.2, 0}, 
									   {0.6, 0.3, 0, 0.1, 0}, 
									   {0.8, 0.1, 0.1, 0}, 
									   {0, 0, 0, 0, 0} };
int upgradePlan[5][4] = { {100, 100, 100, 100},
						  {100, 100, 100, 100},
						  {100, 100, 100, 100},
						  {100, 100, 100, 100},
						  {100, 100, 100, 100} };

areaBlock city[MAX_ROW_SIZE][MAX_COL_SIZE];
int population, popCapacity, blockCount[4];
double populationIncreaseRate, livingScore, overallScore, openSpaceScore, housingScore;
double blockScore[MAX_ROW_SIZE][MAX_COL_SIZE];
double beautyScore[MAX_ROW_SIZE][MAX_COL_SIZE];

void inputData() { ; }

void getBlockInfo() {
	using namespace enumExt;
	for (area::areaType i = area::residence; i < area::unused; i++) blockCount[i] = 0;
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) {
			if (city[i][j].type > area::openspace) {
				blockScore[i][j] = 0;
				continue;
			}
			double temp = 0;
			for (int k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
				for (int l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) temp += city[k][l].development / static_cast<double>(abs(k - i)*abs(k - i) + abs(l - j)*abs(l - j) + 1) * affectParameter[city[i][j].type][city[k][l].type];
			blockScore[i][j] = temp * sqrt(city[i][j].development);
			blockCount[city[i][j].type]++;
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
}

void getOverallScore() {
	using namespace enumExt;
	overallScore = 0;
	double count = 0;
	for (area::areaType i = area::residence; i < area::unused; i++) count += blockCount[i];
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) if (city[i][j].type < area::unused) overallScore += blockScore[i][j];
	overallScore /= count;
}

void getOpenSpaceScore() {
	using namespace enumExt;
	openSpaceScore = 0;
	double count = 0;
	for (area::areaType i = area::residence; i < area::unused; i++) count += blockCount[i];
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) openSpaceScore += beautyScore[i][j];
	openSpaceScore /= count;
}

void getHousingScore() {
	using namespace enumExt;
	housingScore = 0;
	double count = blockCount[area::commerce] + blockCount[area::entertain];
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) if (city[i][j].type == area::commerce || city[i][j].type == area::entertain) {
			int temp = city[i][j].development;
			for (int k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
				for (int l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) if (city[k][l].type == area::residence) temp = min(temp, abs(city[k][l].development - city[i][j].development));
			housingScore += 100 * (1 - temp / static_cast<double>(city[i][j].development));
		}
	housingScore /= count;
}

void upgrade(area::areaType _type, int _limit) {
	bool blockUsed[MAX_ROW_SIZE][MAX_COL_SIZE];
	for (int i = 0; i < MAX_ROW_SIZE; i++)
		for (int j = 0; j < MAX_COL_SIZE; j++) blockUsed[i][j] = false;
	while (_limit > 0) {
		double maxIncrease = 0;
		int x, y, develop;
		for (int i = 0; i < MAX_ROW_SIZE; i++)
			for (int j = 0; j < MAX_COL_SIZE; j++) {
				if (blockUsed[i][j] || city[i][j].type != _type) continue;
				for (int add = 1; add + city[i][j].development <= MAX_LEVEL && add <= _limit; add++) {
					city[i][j].development += add;
					double temp1 = 0;
					for (int k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
						for (int l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) {
							if (k == 0 && l == 0) continue;
							temp1 += add / static_cast<double>(abs(k - i)*abs(k - i) + abs(l - j)*abs(l - j) + 1) * sqrt(city[k][l].development) * affectParameter[city[k][l].type][city[i][j].type];
						}
					temp1 += blockScore[i][j] * (sqrt(city[i][j].development) / sqrt(city[i][j].development - add) - 1);
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
				if (blockUsed[i][j] || city[i][j].type != area::unused) continue;
				for (area::areaType curType = area::residence; curType < area::unused; curType++) {
					for (int add = 0; add  <= MAX_LEVEL && add <= _limit; add++) {
						city[i][j].development = add;
						city[i][j].type = curType;
						double temp1 = 0;
						for (int k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
							for (int l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) {
								if (k == 0 && l == 0) continue;
								temp1 += add / static_cast<double>(abs(k - i)*abs(k - i) + abs(l - j)*abs(l - j) + 1) * sqrt(city[k][l].development) * affectParameter[city[k][l].type][city[i][j].type];
							}
						for (int k = max(0, i - NEIGHBOURHOOD_SIZE); k <= min(MAX_ROW_SIZE - 1, i + NEIGHBOURHOOD_SIZE); k++)
							for (int l = max(0, j - NEIGHBOURHOOD_SIZE); l <= min(MAX_COL_SIZE - 1, j + NEIGHBOURHOOD_SIZE); l++) temp1 += city[k][l].development / static_cast<double>(abs(k - i)*abs(k - i) + abs(l - j)*abs(l - j) + 1) * sqrt(city[i][j].development) * affectParameter[city[i][j].type][city[k][l].type];
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
		if (maxIncrease == 0) break;
		city[x][y].development = develop;
		city[x][y].type = developType;
		blockUsed[x][y] = true;
		_limit -= develop;
	}
}

void upgradeBusStation() {
	using namespace enumExt;
	bool blockUsed[MAX_ROW_SIZE][MAX_COL_SIZE];
	int blockNum, coverNum;
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
	while (coverNum < blockNum) {
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
}

int main() {
	inputData();
	getBlockInfo();
	getBeautyInfo();
	getLivingScore();
	getOverallScore();
	getOpenSpaceScore();
	getHousingScore();
}