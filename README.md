Feature-Data-Displayer
======================
- Interactive Feature data Visualizer
- 마우스로 Feature point를 클릭하면 점이 선택되고, 선택된 점에 해당되는 정보를 Visualize 함
- 점 이외의 영역을 선택하면 전체 Distribution이 보여짐
    - 3가지 전체 Distribution 지원
    - Matching Count 기반 상위 50% & 전체 100% & Repeatability/Similarity 기반 상위 50%
    - Space Bar로 모드 간 전환 가능

![](https://raw.githubusercontent.com/msl-kist/Feature-Data-Displayer/master/screenshots/screenshot.png)

## ofxHistogram
- 이러한 시스템을 구현하기 위하여 ofxHistogram 구현
- __표본을 하나씩 추가__하거나 __한번에 bin에 표본수(count)를 저장__할 수 있음
- cout에 << 연산자로 곧바로 값을 콘솔 화면 상에 출력할 수 있음
- testApp에서 draw(x, y, width, height)로 바로 그려볼 수 있음 
