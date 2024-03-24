using System;
using Monado;

namespace FPSExample
{
	public class FPSPlayer : Entity
	{
		public float WalkingSpeed = 10.0F;
		public float RunSpeed = 20.0F;
		public float JumpForce = 50.0F;

		private bool m_Colliding = false;
		private float m_CurrentSpeed;

		private RigidBodyComponent m_RigidBody;
		private TransformComponent m_Transform;

		private Entity m_CameraEntity;

		void OnCreate()
		{
			m_Transform = GetComponent<TransformComponent>();
			m_RigidBody = GetComponent<RigidBodyComponent>();

			m_CurrentSpeed = WalkingSpeed;

			AddCollisionBeginCallback((n) => { m_Colliding = true; });
			AddCollisionEndCallback((n) => { m_Colliding = false; });

			m_CameraEntity = FindEntityByTag("Camera");
		}

		void OnUpdate(float ts)
		{
			m_CurrentSpeed = Input.IsKeyPressed(KeyCode.LeftControl) ? RunSpeed : WalkingSpeed;

			if (Input.IsKeyPressed(KeyCode.W))
				m_RigidBody.AddForce(m_Transform.Forward * m_CurrentSpeed);
			else if (Input.IsKeyPressed(KeyCode.S))
				m_RigidBody.AddForce(m_Transform.Forward * -m_CurrentSpeed);

			if (Input.IsKeyPressed(KeyCode.A))
				m_RigidBody.AddForce(m_Transform.Right * -m_CurrentSpeed);
			else if (Input.IsKeyPressed(KeyCode.D))
				m_RigidBody.AddForce(m_Transform.Right * m_CurrentSpeed);

			if (Input.IsKeyPressed(KeyCode.Space) && m_Colliding)
				m_RigidBody.AddForce(Vector3.Up * JumpForce);

			// TODO: This workflow needs to be improved. (Will be fixed by object parenting)
			Matrix4 cameraTransform = m_CameraEntity.GetTransform();
			Vector3 cameraTranslation = cameraTransform.Translation;
			Vector3 translation = m_Transform.Transform.Translation;
			cameraTranslation.XZ = translation.XZ;
			cameraTranslation.Y = translation.Y + 1.5F;
			cameraTransform.Translation = cameraTranslation;
			m_CameraEntity.SetTransform(cameraTransform);
		}
	}
}
