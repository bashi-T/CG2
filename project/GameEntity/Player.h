#include"objects/object3d.h"
#include"Commons/Object3dCommon.h"
#include"Managers/SRVManager.h"
#include"Input/Input.h"

class Player
{
public:
	void Initialize();
	void Update();
	void Draw();

private:
	Object3d* object3d;


};

