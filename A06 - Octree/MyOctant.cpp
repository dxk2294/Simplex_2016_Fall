#include "MyOctant.h"
#include "DJKEntityManager.h"


using namespace Simplex;

	uint MyOctant::uMyOctantCount = 0;
	uint MyOctant::uMaxLevel = 0;
	uint MyOctant::uIdealEntityCount = 0;

	Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
	{
		uMaxLevel = a_nMaxLevel;
		uIdealEntityCount = a_nIdealEntityCount;

		m_pMeshMngr = MeshManager::GetInstance();
		m_pEntityMngr = DJKEntityManager::GetInstance();

		for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++) {
			m_EntityList.push_back(i);
		}

		m_fSize = 72.0f;
		m_v3Center = vector3(0.0f);
		m_v3Min = -1.0f * vector3(m_fSize / 2.0f);
		m_v3Max = vector3(m_fSize / 2.0f);

		GenerateModelMatrix();

		m_uChildren = 0;
		m_uLevel = 0;

		m_pRoot = this;

		m_uID = uMyOctantCount;
		uMyOctantCount += 1;
		m_pRoot->m_lChildAndEmpty.push_back(this);

		Subdivide();
	}

	Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize, MyOctant* parent)
	{
		m_pParent = parent;
		m_uID = uMyOctantCount;
		m_uLevel = parent->m_uLevel + 1;

		uMyOctantCount += 1;

		m_pMeshMngr = MeshManager::GetInstance();
		m_pEntityMngr = DJKEntityManager::GetInstance();

		m_fSize = a_fSize;
		m_v3Center = a_v3Center;
		m_v3Min = a_v3Center - vector3(m_fSize / 2.0f);
		m_v3Max = a_v3Center + vector3(m_fSize / 2.0f);

		GenerateModelMatrix();

		m_pRoot = m_pParent->m_pRoot;
		m_pRoot->m_lChildAndEmpty.push_back(this);
	}

	Simplex::MyOctant::MyOctant(MyOctant const & other)
	{
		uMyOctantCount += 1;

		GenerateModelMatrix();
	}

	MyOctant & Simplex::MyOctant::operator=(MyOctant const & other)
	{
		// TODO: insert return statement here
		return *this;
	}

	Simplex::MyOctant::~MyOctant(void)
	{
		Release();
	}

	void Simplex::MyOctant::Swap(MyOctant & other)
	{
	}

	float Simplex::MyOctant::GetSize(void)
	{
		return m_fSize;
	}

	vector3 Simplex::MyOctant::GetCenterGlobal(void)
	{
		return m_v3Center;
	}

	vector3 Simplex::MyOctant::GetMinGlobal(void)
	{
		return m_v3Min;
	}

	vector3 Simplex::MyOctant::GetMaxGlobal(void)
	{
		return m_v3Max;
	}

	bool Simplex::MyOctant::IsColliding(uint a_uRBIndex)
	{
		bool bColliding = true;

		MyRigidBody* rb = m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody();

		vector3 rb_MinG = rb->GetMinGlobal();
		vector3 rb_MaxG = rb->GetMaxGlobal();

		if (this->m_v3Max.x < rb_MinG.x) //this to the right of other
			bColliding = false;
		if (this->m_v3Min.x > rb_MaxG.x) //this to the left of other
			bColliding = false;

		if (this->m_v3Max.y < rb_MinG.y) //this below of other
			bColliding = false;
		if (this->m_v3Min.y > rb_MaxG.y) //this above of other
			bColliding = false;

		if (this->m_v3Max.z < rb_MinG.z) //this behind of other
			bColliding = false;
		if (this->m_v3Min.z > rb_MaxG.z) //this in front of other
			bColliding = false;

		if (bColliding) {
			bColliding = true;
		}

		return bColliding;
	}

	void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
	{
		if (m_lChildAndEmpty.size() == 0) return;

		if (a_nIndex >= m_lChildAndEmpty.size()) {
			a_nIndex = m_lChildAndEmpty.size() - 1;
		}
		m_lChildAndEmpty[a_nIndex]->Display(a_v3Color);
	}

	void Simplex::MyOctant::Display(vector3 a_v3Color, bool displayLeafs)
	{
		m_pMeshMngr->AddWireCubeToRenderList(modelMatrix, a_v3Color);

		if (displayLeafs) {
			DisplayLeafs(a_v3Color);
		}
	}

	void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
	{
		if (!IsLeaf()) {
			for (int i = 0; i < 8; i++) {
				m_pChild[i]->Display(a_v3Color, true);
			}
		}
	}

	void Simplex::MyOctant::ClearEntityList(void)
	{
		if (IsLeaf()) {
			m_EntityList.clear();
		}
		else {
			for (int i = 0; i < 8; i++) {
				m_pChild[i]->ClearEntityList();
			}
		}
	}

	void Simplex::MyOctant::Subdivide(void)
	{
		if (m_uLevel >= uMaxLevel || m_EntityList.size() <= uIdealEntityCount) {
			if (m_EntityList.size() > 0) {
				m_pRoot->m_lChild.push_back(this);
			}
			return;
		}


		float size = m_fSize / 2.0f;
		float offset = size / 2.0f;
		m_pChild[0] = new MyOctant(m_v3Center + vector3(offset, offset, offset), size, this);
		m_pChild[1] = new MyOctant(m_v3Center + vector3(-offset, offset, offset), size, this);
		m_pChild[2] = new MyOctant(m_v3Center + vector3(-offset, offset, -offset), size, this);
		m_pChild[3] = new MyOctant(m_v3Center + vector3(offset, offset, -offset), size, this);

		m_pChild[4] = new MyOctant(m_v3Center + vector3(offset, -offset, offset), size, this);
		m_pChild[5] = new MyOctant(m_v3Center + vector3(-offset, -offset, offset), size, this);
		m_pChild[6] = new MyOctant(m_v3Center + vector3(-offset, -offset, -offset), size, this);
		m_pChild[7] = new MyOctant(m_v3Center + vector3(offset, -offset, -offset), size, this);

		m_uChildren = 8;

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < m_EntityList.size(); j++) {
				if (m_pChild[i]->IsColliding(m_EntityList[j])) {
					m_pChild[i]->m_EntityList.push_back(m_EntityList[j]);
				}
			}
		}

		for (int i = 0; i < 8; i++) {
			m_pChild[i]->Subdivide();
		}

		m_EntityList.clear();
	}

	MyOctant * Simplex::MyOctant::GetChild(uint a_nChild)
	{
		return m_pChild[a_nChild];
	}

	MyOctant * Simplex::MyOctant::GetParent(void)
	{
		return m_pParent;
	}

	bool Simplex::MyOctant::IsLeaf(void)
	{
		return m_uChildren == 0;
	}

	bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
	{
		return m_EntityList.size() > a_nEntities;
	}

	void Simplex::MyOctant::KillBranches(void)
	{
	}

	uint Simplex::MyOctant::GetMyOctantCount(void)
	{
		return uMyOctantCount;
	}

	void MyOctant::GenerateModelMatrix(void)
	{
		modelMatrix = glm::translate(m_v3Center);
		modelMatrix = glm::scale(modelMatrix, vector3(m_fSize, m_fSize, m_fSize));
	}

	std::vector<MyOctant*> Simplex::MyOctant::GetLeavesWithEntities(void)
	{
		return m_lChild;
	}

	std::vector<uint> Simplex::MyOctant::GetEntities(void)
	{
		return m_EntityList;
	}

	uint Simplex::MyOctant::GetMaxDepth(void)
	{
		return MyOctant::uMaxLevel;
	}

	void Simplex::MyOctant::Release(void)
	{
		if (m_pParent == nullptr) {
			// we are root
			for (int i = 1; i < m_lChildAndEmpty.size(); i++) {
				if (m_lChildAndEmpty[i] != nullptr) {
					delete m_lChildAndEmpty[i];
				}
			}
		}
	}

	void Simplex::MyOctant::Init(void)
	{
	}

	void Simplex::MyOctant::ConstructList(void)
	{
		if (IsLeaf()) {
			m_pRoot->m_lChild.push_back(this);
		} else {
			for (int i = 0; i < 8; i++) {
				m_pChild[i]->ConstructList();
			}
		}
	}
