using System;
using Monado;

public class RuntimeMesh : Entity
{

	public Mesh MyMesh;

	void OnCreate()
	{
		GetComponent<MeshComponent>().Mesh = MyMesh;
	}

}
