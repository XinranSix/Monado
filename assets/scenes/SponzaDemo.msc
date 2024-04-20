Scene: Scene Name
Environment:
  AssetPath: assets\env\birchwood_4k.hdr
  Light:
    Direction: [0, 0, 0]
    Radiance: [0, 0, 0]
    Multiplier: 1
Entities:
  - Entity: 13882838760121718506
    TagComponent:
      Tag: Directional Light
    TransformComponent:
      Position: [0, 0, 0]
      Rotation: [2.35444903, 0, -0.441568285]
      Scale: [1, 1, 1]
    DirectionalLightComponent:
      Radiance: [1, 1, 1]
      CastShadows: true
      SoftShadows: true
      LightSize: 0.5
  - Entity: 2143608321399101581
    TagComponent:
      Tag: Sky Light
    TransformComponent:
      Position: [0, 0, 0]
      Rotation: [0, 0, 0]
      Scale: [1, 1, 1]
    SkyLightComponent:
      EnvironmentAssetPath: assets\env\birchwood_4k.hdr
      Intensity: 1
      Angle: 0
  - Entity: 5471747392929574277
    TagComponent:
      Tag: Sponza
    TransformComponent:
      Position: [0, 0, 0]
      Rotation: [0, 0, 0]
      Scale: [0.00999999978, 0.00999999978, 0.00999999978]
    MeshComponent:
      AssetPath: assets\meshes\sponza\sponza.obj
    RigidBodyComponent:
      BodyType: 0
      Mass: 1
      LinearDrag: 0
      AngularDrag: 0.0500000007
      DisableGravity: false
      IsKinematic: false
      Layer: 0
      Constraints:
        LockPositionX: false
        LockPositionY: false
        LockPositionZ: false
        LockRotationX: false
        LockRotationY: false
        LockRotationZ: false
    PhysicsMaterialComponent:
      StaticFriction: 0.5
      DynamicFriction: 0.25
      Bounciness: 0.100000001
    MeshColliderComponent:
      IsConvex: false
      IsTrigger: false
      OverrideMesh: false
  - Entity: 5561134054991576534
    TagComponent:
      Tag: Player
    TransformComponent:
      Position: [0, 6.31091785, 4.11461163]
      Rotation: [0, 0, 0]
      Scale: [1, 1, 1]
    ScriptComponent:
      ModuleName: FPSExample.FPSPlayer
      StoredFields:
        - Name: WalkingSpeed
          Type: 1
          Data: 3
        - Name: RunSpeed
          Type: 1
          Data: 5
        - Name: JumpForce
          Type: 1
          Data: 1
        - Name: CameraForwardOffset
          Type: 1
          Data: 0.200000003
        - Name: CameraYOffset
          Type: 1
          Data: 0.850000024
        - Name: MouseSensitivity
          Type: 1
          Data: 2
    MeshComponent:
      AssetPath: assets\meshes\Capsule.fbx
    RigidBodyComponent:
      BodyType: 1
      Mass: 1
      LinearDrag: 0
      AngularDrag: 0.0500000007
      DisableGravity: false
      IsKinematic: false
      Layer: 0
      Constraints:
        LockPositionX: false
        LockPositionY: false
        LockPositionZ: false
        LockRotationX: true
        LockRotationY: true
        LockRotationZ: true
    PhysicsMaterialComponent:
      StaticFriction: 0.100000001
      DynamicFriction: 1
      Bounciness: 0
    MeshColliderComponent:
      IsConvex: true
      IsTrigger: false
      OverrideMesh: false
  - Entity: 12392346523579991874
    TagComponent:
      Tag: Camera
    TransformComponent:
      Position: [0, 1.0587908, 0]
      Rotation: [0, 0, 0]
      Scale: [1, 1, 1]
PhysicsLayers:
  []