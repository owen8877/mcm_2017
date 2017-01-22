* Mix Land Use Score
  $$
  blockScore[0][0] =\sum_{k\in areaType} \\
  (\sum_{area[i][j].type ==k , |i| \leq 2 ,|j| \leq2}\frac{area[i][j].development}{i^2+j^2+1})*\sqrt{area[0][0].development}*f(residence, k) \\
  rawlivingScore = (\sum blockScore[i][j])/residenceBlockCount\\
  $$



* Open Space Score
  $$
  beautyScore[0][0] = \sum_{area[i][j].type ==openSpace , |i| \leq 2 ,|j| \leq2}\frac{area[i][j].development}{i^2+j^2+1} \\
  rawOpenSpaceScore = (\sum beautyScore[i][j] )/usedBlockCount(except \ openspace)
  $$



* Housing Score
  $$
  \scriptsize fitScore[0][0] = \min\left\{area[0][0].development, |area[i][j]-area[0][0]| \normalsize | \scriptsize |i| \leq 2, |j| \leq 2,area[i][j].type == residence \right\} \\
  fitScore[0][0] = 100 * (1-fitScore[0][0]/area[0][0].development) \\
  housingScore = (\sum fitScore[i][j] )/commerce \ and \ entertainCount
  $$



* Adjust Function

  * We use
    $$
    y = \frac{100rx + k}{rx+1} \triangleq f(r,k,x)
    $$
    ​


  * livingScore:

    We suppose that only 10 percent blocks are openspace. 

    60 points : when every block's development equals to 1and blocks' types are completely random.
    $$
    rawLivingScore =\sum_{-2 \leq i \leq 2 \ and \ -2 \leq j \leq2 \ and \ (i,j) \not =(0,0)}\frac{1}{i^2+j^2+1}*0.1=0.591111 \\
    livingScore = 60
    $$
    85 points : when every block's development equals to 2 and blocks' types are completely random. 
    $$
    rawLivingScore =\sum_{-2 \leq i \leq 2 \ and \ -2 \leq j \leq2 \ and \ (i,j) \not =(0,0)}\frac{2}{i^2+j^2+1}*0.1=1.18222 \\
    livingScore = 85
    $$
    We thus have :
    $$
    r_l = -4.22932 \\
    k_l = 160 \\
    livingScore = f(r_l,k_l,rawLivingScore)
    $$

  * openspaceScore:

    60 points : when every block's development equals to 1and blocks' types are completely random.
    $$
    rawOpenspaceScore =\sum_{-2 \leq i \leq 2 \ and \ -2 \leq j \leq2 \ and \ (i,j) \not =(0,0)}\frac{1}{i^2+j^2+1}*\sqrt{1}*0.25=1.47778 \\
    openspaceScore = 60
    $$
    85 points : when every block's development equals to 2 and blocks' types are completely random. 
    $$
    rawOpenspaceScore =\sum_{-2 \leq i \leq 2 \ and \ -2 \leq j \leq2 \ and \ (i,j) \not =(0,0)}\frac{2}{i^2+j^2+1}*\sqrt{2}*0.25=4.17979 \\
    openspaceScore = 85
    $$
    We thus have :
    $$
    r_o = 6.97216 \\
    k_o = -352.132 \\
    openspaceScore = f(r_o,k_o,rawOpenspaceScore)
    $$

* Develop or Upgrade Willing Point
  $$
  blockPoint[i][j][add] = \\
  \Delta(livingScore + openSpaceScore + housingScore)/(add * area[i][j].development) \\
  =\Delta(rawLivingScore)\frac{\partial f(r_l,k_l,rawLivingScore)}{\partial rawLivingScore} + \\ \Delta(rawOpenspaceScore)\frac{\partial f(r_o,k_o,rawOpenspaceScore)}{\partial rawOpenspaceScore} +
  \Delta(housingScore)
  $$



