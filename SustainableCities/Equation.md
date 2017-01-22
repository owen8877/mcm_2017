* Mix Land Use Score
  $$
  blockScore[0][0] =\sum_{k\in areaType} \\
  (\sum_{area[i][j].type ==k , |i| \leq 4 ,|j| \leq4}\frac{area[i][j].development}{i^2+j^2+1})*\sqrt{area[0][0].development}*f(residence, k) \\
  livingScore = (\sum blockScore[i][j])/residenceBlockCount\\
  $$



* Open Space Score
  $$
  beautyScore[0][0] = \sum_{area[i][j].type ==openSpace , |i| \leq 4 ,|j| \leq4}\frac{area[i][j].development}{i^2+j^2+1} \\
  openSpaceScore = (\sum beautyScore[i][j] )/usedBlockCount
  $$



* Housing Score
  $$
  \scriptsize fitScore[0][0] = \min\left\{area[0][0].development, |area[i][j]-area[0][0]| \normalsize | \scriptsize |i| \leq 4, |j| \leq 4 ,area[i][j].type == residence \right\} \\
  fitScore[0][0] = 100 * (1-fitScore[0][0]/area[0][0].development) \\
  housingScore = (\sum fitScore[i][j] )/commerceandentertainCount
  $$



* Develop or Upgrade Willing Point
  $$
  blockPoint[i][j][add] =\Delta(livingScore + openSpaceScore + housingScore)/(add * area[i][j].development) \\
  $$


