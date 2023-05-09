<div align="center">

# [DirectX9] Don't Starve 모작

<img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/957764c3-da73-41b2-850c-7fbaa93082aa" width="50%"/>

##### **📌해당 설명서는 팀원 모두의 구현 내용을 담은 것이 아닌, 작성자 개인이 담당한 파트 일부분만을 따로 기록한 것입니다. 참고바랍니다.**
  
</div>

<br></br>

## *동영상/GitHub*

[[Don't Starve 모작]쥬신게임아카데미 게임프로그래밍학원 6개월차 SR 팀 시연회_130기 2조 작품_원혜연 서정원 최우진 조이_클라이언트](https://www.youtube.com/watch?v=GGm85Jgj680)

# *담당파트*

- ***조장***
- ***프레임워크***
- ***카메라***
- ***쉐이더***
- ***기타 오브젝트***
- ***맵툴***

# *게임 주요 사진*


### 스테이지 구성

스테이지는 크게 메인필드, 거미 던전, 미로 던전, 보스룸으로 구성되어있습니다.

<img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/1046eec6-86b4-4ba9-982c-f8e2a6cdf4b1" width="40%"/> <img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/00e589e9-94bf-4bee-a377-86a32e85113e" width="40%"/>

<img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/47611cdb-a963-4156-8509-ee94c37acd54" width="40%"/> <img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/ec5679fb-475e-4543-ab04-6b50a5429f99" width="40%"/>


# *구현 컨텐츠 및 기능 설명*


## 1. 프레임워크

<div align="center">
<img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/e6a2eeb3-3ef8-4a60-9883-12d0602b5753" width="70%"/>

<img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/850c1d76-1d8a-465d-8adc-3cb805d7fc3f" width="70%"/>

</div>

- 게임 플레이를 위한 Client와 공통적인 기능들을 담은 Engine으로 **두 개의 프로젝트를 생성**했습니다. 클라이언트에서 여러 객체들이 사용할 수 있는 struct, function, 매크로 등을 모두 Engine프로젝트에서 생성했습니다.
- 그 밖에도 **Engine 프로젝트에서 레퍼런스를 관리하기 위한 추상 클래스를 만들어 공통적인 기능들을 수행하게 했습니다.** Engine 프로젝트 내부에서 Initialize - Tick - Late Tick - Render - Free등 공통 기능들을 구현하고, 클라이언트에서 추상 클래스의 함수를 오버라이드 하여 세부 기능들을 구현하게 처리했습니다.
- 이와 같은 엔진 프로젝트는 **DLL(동적 라이브러리 파일)로 변환**하여 클라이언트에서 기능을 사용할 수 있도록 처리했습니다.
- 엔진에 있는 CGameInstance를 싱글톤으로 처리하여 엔진 내부에서 Tick→LateTick→Render를 선언하였고, 그 안에서 다른 공통적인 기능들을 담당하는 Collision Manager,  Level_Manager, Object_Manaer, Sound_Manager, Timer_Manager 등을 싱글톤으로 구현하여 각각의 매니저가 가지고 있는 오브젝트들을 업데이트할 수 있도록 처리했습니다.
- 객체들을 생성할 때 레퍼런스 관리를 위해 추상 클래스를 생성하고, 상속 클래스를 생성할 때 추상 클래스의 프로토타입을 클론하는 **Prototype 패턴**을 사용했습니다.
- 또한 공통적으로 필요한 기능들(예를 들어 VIBuffer, Transform, Collision 등등)은 엔진 프로젝트에 **컴포넌트 패턴**으로 구현하여 각 객체마다 필요한 컴포넌트들을 할당하게 처리했습니다.
- 레벨 전환 시 **로딩씬에서 쓰레드를 이용**하여 각 레벨에 필요한 프로토타입을 생성하고, 필요없는 프로토타입은 삭제되게 처리했습니다.

## 2. 카메라


<img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/9a0bd314-cf9f-4a5f-b3d5-b118f548d719" width="50%"/>

- 엔진에서 추상 클래스인 카메라를 생성하고, 클라이언트에서 상속 클래스로 Dynamic 카메라, FPS 카메라, Target 카메라 총 3개의 카메라를 구현했습니다.
- 이는 싱글톤으로 구현한 Camera Manager 를 통해 실행할 카메라를 선택할 수 있습니다.

### Dynamic Camera

![제목 없는 동영상 - Clipchamp로 제작](https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/73d370ec-9450-4c6f-8119-0ad194b339d3)

- Dynamic 카메라는 플레이어 기준 카메라로, 마우스 휠로 ZoomIn/ZoomOut 할 수 있습니다.
- Q와 E키를 통해 카메라를 90도 회전할 수 있으며, 방향키 조절을 통해 카메라 높낮이 각도를 조절할 수 있습니다.

### FPS 카메라

![제목 없는 동영상 - Clipchamp로 제작 (1)](https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/175c3a8a-c222-4a0e-81b5-836c640e1338)


- FPS 카메라는 1인칭 시점으로 Q와 E를 통해 각도를 회전할 수 있습니다. 동시에 플레이어의 Look Vector 도 카메라의 Look Vector를 따라가게 설정했습니다.

### Target Camera


<img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/cc250921-c1d0-4991-9286-89135ab18885" width="30%"/>
<img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/f63ceab2-7050-4282-b444-27d2be16aa87" width="30%"/>
<img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/ec33f44c-895f-40c8-ad3c-13bdcdbe3f7a" width="30%"/>


- Target Camera는 스토리 진행을 위해 작동되는 카메라입니다.
- 특정 보스 몬스터를 보여주는 씬이나, 특정 좌표를 바라봐야할 때, 혹은 NPC와 대화를 할 때 해당 카메라를사용합니다.
- 보스 몬스터의 경우 전투 시 카메라 쉐이킹을 함께 적용해주었습니다.

## 3. 쉐이더

<img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/d16ab8ee-f046-4009-bc54-2d6b953e6730" width="30%"/>
<img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/f055e000-8f48-4775-9631-37a14debb743" width="30%"/>

- **옵저버 패턴**을이용해서 시간의 흐름을 기록하고, DAY,DINNER,NIGHT로 시간의 흐름이 바뀔 때마다 전체 객체들의 쉐이더 처리를 공통적으로 바꿀 수 있도록 처리했습니다. 시간의 흐름이 바뀔 때마다 플레이어의 스탯 변화, 몬스터들의 등장 변화 등 또한 옵저버 패턴을 이용했습니다.
- 밤이 될 경우 플레이어의 좌표를 기준으로 Min과 Max 범위를 설정해 Fog 효과를 주어 횃불효과를 주었습니다.

![제목 없는 동영상 - Clipchamp로 제작 (7)](https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/5ef8bf4d-2d85-4cf7-9b32-a80cf1384487)

- 시간의 흐름 이외에도 **플레이어의 상태에 따른 쉐이더 처리**를 구현했습니다. 배고픔, 정신력, 체력중 하나라도 0이되어 사망할 경우 쉐이더로 rgb 값을 조절하여 흑백으로 효과를 주었습니다. 부활하면 다시 원래 색으로 돌아오게 처리했습니다.
    
   
 ![제목 없는 동영상 - Clipchamp로 제작 (12)](https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/50e5e1f0-6d27-40de-9122-b6d072d106d2)

    
- 그 외에도 아이템 설치 시 마우스 피킹으로 터레인 좌표를 구해 아이템을 설치하는 기능을 구현했고, 쉐이더로 알파블렌딩 처리하여, 설치할 공간을 미리 확인할 수 있도록 했습니다.
    
  
    

## 4. 미로 던전 컨텐츠

9개의 방으로 맵을 형성하고 각 방에 입장 할 때마다 문을 잠궈 해당 방 안에 있는 미니 컨텐츠를 진행하게했습니다. 

컨텐츠를 통과해야만 다음 방으로 넘어갈수 있는 구조로 설치했습니다. 

### 미로 탈출

- 특정 좌표 범위에 도달하면 시점이 FPS 카메라로 전환되어 1인칭 시점으로 바뀝니다. 해당 공간에서 함정에 걸리지 않고 탈출 해야합니다.
- 미로 중간중간에 맵툴로 함정을 설치하여 플레이어의 이동경로를 제어하고 혼돈을 주도록 했습니다.

### 슈팅 게임

- 슈팅 게임을 시작하면 Target Camera로 카메라 뷰가 바뀌며 게임이 시작됩니다.
- y축 회전 행렬을 이용하여 화살을 슈터 기준으로 공전시켜, 버튼을 누르면 화살이 발사되게 처리했습니다.
- 타겟은 새와 파리로 랜덤으로 변동되며, 5개 타겟 모두 새로 명중시킬 시 게임이 종료되게 처리했습니다.

### 거미 디펜스

- 디펜스할 석상과, 몬스터 스포너를 설치하여 디펜스 게임을 진행했습니다.스포너는 총 5번 몬스터를 소환할 수 있고, 플레이어가 일정 거리에 가까워지면 생성합니다.
- 몬스터는 석상을 공격하려하고, 플레이어는 스포너의 모든 몬스터가 죽을 때까지 석상을 지키면서 몬스터를 해치워야합니다.

### 둥지로 알보내기

- 게임 시작시 계란들이 지정된 좌표로 등장하며 플레이어로부터 도망칩니다.
- 플레이어는 이를 고려하여 알의 반대 방향으로 움직여 이동 경로를 제어해 둥지로 유인해야합니다.
- 알은 15초마다 재생성되며, 모든 알들이 둥지로 돌아갔을 경우 문이 열리며 게임이 종료됩니다.

## 5. 환경 오브젝트/파티클

전반적인 환경 오브젝트들을 설치하고 디테일을 구현했습니다.

<img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/df86a088-364d-4e4e-90fa-229e954e652f" width="30%"/>
<img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/e34a52ab-cadc-45c8-8495-6457e084d266" width="30%"/>
<img src="https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/9c5d412f-7e6d-468e-be17-9f1db47472d6" width="30%"/>


## 6. MFC 맵툴

![제목 없는 동영상 - Clipchamp로 제작 (16)](https://github.com/WANTWON/CopyGame_DirectX9_Don-t_Starve/assets/106663427/a2338bda-38de-4739-921a-cb57abdf9453)


- MFC로 맵툴을 만들어 터레인을 설치하고, 높이를 조절하는 기능을 구현했습니다.
- 추가적으로 object를 설치하여 해당 object의 종류와 위치 등을 저장하여 클라이언트에 연동했습니다.

## *기술적 요소*

자세한 코드 설명은 깃허브 프로젝트 혹은 기술소개서에서 확인 가능합니다.

- 옵저버 패턴, 프로토타입 패턴, 컴포넌트 패턴
- AABB, OBB 큐브 충돌
- 파티클 시스템
- 카메라 변환
- 절두체 컬링
- MFC 맵툴
