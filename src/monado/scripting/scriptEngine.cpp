// #include "monado/scripting/scriptEngine.h"
// #include "monado/scripting/scriptGlue.h"

// #include "glm/gtc/matrix_transform.hpp"

// #include "mono/jit/jit.h"
// #include "mono/metadata/assembly.h"
// #include "mono/metadata/object.h"
// // #include "mono/metadata/tabledefs.h"

// namespace Monado {

//     static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap = {
//         { "System.Single", ScriptFieldType::Float },    { "System.Double", ScriptFieldType::Double },
//         { "System.Boolean", ScriptFieldType::Bool },    { "System.Char", ScriptFieldType::Char },
//         { "System.Int16", ScriptFieldType::Short },     { "System.Int32", ScriptFieldType::Int },
//         { "System.Int64", ScriptFieldType::Long },      { "System.Byte", ScriptFieldType::Byte },
//         { "System.UInt16", ScriptFieldType::UShort },   { "System.UInt32", ScriptFieldType::UInt },
//         { "System.UInt64", ScriptFieldType::ULong },

//         { "System.Vector2", ScriptFieldType::Vector2 }, { "System.Vector3", ScriptFieldType::Vector3 },
//         { "System.Vector4", ScriptFieldType::Vector4 },

//         { "Hazel.Entity", ScriptFieldType::Entity },
//     };

//     namespace Utils {
//         char *ReadBytes(const std::filesystem::path &filepath, uint32_t *outSize) {
//             std::ifstream stream(filepath, std::ios::binary | std::ios::ate);
//             if (!stream) {
//                 // ���ļ�ʧ��
//                 return nullptr;
//             }
//             std::streampos end = stream.tellg();
//             stream.seekg(0, std::ios::beg);
//             uint32_t size = end - stream.tellg();
//             if (size == 0) {
//                 // �ļ��ǿ�
//                 return nullptr;
//             }
//             char *buffer = new char[size];
//             stream.read((char *)buffer, size); // ����char�ַ�������
//             stream.close();

//             *outSize = size; // ָ�뷵�ش�С
//             return buffer;   // �����ַ��������λ��
//         }
//         MonoAssembly *LoadCSharpAssembly(const std::filesystem::path &assemblyPath) {
//             uint32_t fileSize = 0;
//             char *fileData = ReadBytes(assemblyPath, &fileSize);

//             // ���˼��س���֮�⣬���ǲ��ܽ���ͼ��image�����κ�������;����Ϊ��ͼ��û�жԳ��򼯵�����
//             MonoImageOpenStatus status;
//             MonoImage *image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

//             if (status != MONO_IMAGE_OK) {
//                 const char *erroMessage = mono_image_strerror(status);
//                 // ���Դ�ӡ������Ϣ
//                 return nullptr;
//             }
//             std::string pathString = assemblyPath.string();
//             MonoAssembly *assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
//             mono_image_close(image);

//             // �ͷ��ڴ�
//             delete[] fileData;
//             return assembly;
//         }
//         void PrintAssemblyTypes(MonoAssembly *assembly) {
//             // ��ӡ���ص�c#�������Ϣ
//             MonoImage *image = mono_assembly_get_image(assembly);
//             const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
//             int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

//             for (int32_t i = 0; i < numTypes; i++) {
//                 uint32_t cols[MONO_TYPEDEF_SIZE];
//                 mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

//                 const char *nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
//                 const char *name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

//                 MND_CORE_TRACE("{}.{}", nameSpace, name); // �����ռ������
//             }
//         }
//         // ��������Ϊ����C#����������ת��Ϊ�Զ����������(��C++���������ƣ����ױ�ʶ
//         ScriptFieldType MonoTypeToScriptFieldType(MonoType *monoType) {
//             std::string typeName = mono_type_get_name(monoType);
//             auto it = s_ScriptFieldTypeMap.find(typeName);
//             if (it == s_ScriptFieldTypeMap.end()) {
//                 MND_CORE_ERROR("Unknown type:{}", typeName);
//                 return ScriptFieldType::None;
//             }
//             return it->second;
//         }
//         // ���Զ����������ת��Ϊ�ַ���
//         const char *ScriptFieldTypeToStirng(ScriptFieldType type) {
//             switch (type) {
//             case ScriptFieldType::Float: return "Float";
//             case ScriptFieldType::Double: return "Double";
//             case ScriptFieldType::Bool: return "Bool";
//             case ScriptFieldType::Char: return "Char";
//             case ScriptFieldType::Byte: return "Byte";
//             case ScriptFieldType::Short: return "Short";
//             case ScriptFieldType::Int: return "Int";
//             case ScriptFieldType::Long: return "Long";
//             case ScriptFieldType::UByte: return "UByte";
//             case ScriptFieldType::UShort: return "UShort";
//             case ScriptFieldType::UInt: return "UInt";
//             case ScriptFieldType::ULong: return "ULong";
//             case ScriptFieldType::Vector2: return "Vector2";
//             case ScriptFieldType::Vector3: return "Vector3";
//             case ScriptFieldType::Vector4: return "Vector4";
//             case ScriptFieldType::Entity: return "Entity";
//             }
//             return "<Invalid>";
//         }
//     } // namespace Utils
//     struct ScriptEngineData {
//         MonoDomain *RootDomain = nullptr;
//         MonoDomain *AppDomain = nullptr;

//         MonoAssembly *CoreAssembly = nullptr;
//         MonoImage *CoreAssemblyImage = nullptr;

//         MonoAssembly *AppAssembly = nullptr;
//         MonoImage *AppAssemblyImage = nullptr;

//         ScriptClass EntityClass; // �洢Entity����

//         std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses; // ����C#�ű�map
//         std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;   // ��Ҫ���е�C#�ű�map

//         // Runtime
//         Scene *SceneContext = nullptr; // ���������ģ�����C#����C++�ڲ�����ʱ����UUID��ȡ���������ʵ��
//     };
//     static ScriptEngineData *s_Data = nullptr;

//     //////////////////////////////////////////////////////////////
//     // ScriptEngine////////////////////////////////////////////////
//     //////////////////////////////////////////////////////////////
//     void ScriptEngine::Init() {
//         s_Data = new ScriptEngineData();
//         // ��ʼ��mono
//         InitMono();
//         // ����c#����
//         LoadAssembly("Resources/Scripts/GameEngine-ScriptCore.dll");           // ���Ŀ�
//         LoadAppAssembly("SandboxProject/Assets/Scripts/Binaries/Sandbox.dll"); // ��Ϸ�ű���

//         // ���ظ�����entity�Ľű���
//         LoadAssemblyClasses();

//         // ��������Entity����-Ϊ���ڵ���OnCreate����֮ǰ��UUID����C#Entity�Ĺ��캯��
//         s_Data->EntityClass = ScriptClass("Hazel", "Entity", true);

//         // �����ڲ�����
//         ScriptGlue::RegisterFunctions();
//         ScriptGlue::RegisterComponents();

//         // HZ_CORE_ASSERT(false);
//     }
//     void ScriptEngine::Shutdown() {
//         ShutdownMono();
//         delete s_Data;
//     }
//     void ScriptEngine::InitMono() {
//         // ���ó���װ��·��(���Ƶ�4.5�汾��·��)
//         mono_set_assemblies_path("mono/lib");

//         MonoDomain *rootDomian = mono_jit_init("HazelJITRuntime");
//         MND_CORE_ASSERT(rootDomian);

//         // �洢root domainָ��
//         s_Data->RootDomain = rootDomian;
//     }
//     void ScriptEngine::LoadAssembly(const std::filesystem::path &filepath) {
//         // ����һ��app domain
//         s_Data->AppDomain = mono_domain_create_appdomain("HazelScriptRuntime", nullptr);
//         mono_domain_set(s_Data->AppDomain, true);

//         // ����c#��Ŀ������dll
//         s_Data->CoreAssembly = Utils::LoadCSharpAssembly(filepath);
//         s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
//         // Utils::PrintAssemblyTypes(s_Data->CoreAssembly);// ��ӡdll�Ļ�����Ϣ
//     }
//     void ScriptEngine::LoadAppAssembly(const std::filesystem::path &filepath) {
//         // ����c#��Ŀ������dll
//         s_Data->AppAssembly = Utils::LoadCSharpAssembly(filepath);
//         s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
//         // Utils::PrintAssemblyTypes(s_Data->CoreAssembly);// ��ӡdll�Ļ�����Ϣ
//     }
//     void ScriptEngine::ShutdownMono() {
//         // ��mono��ж���е��Ժ����Ժ��ٽ��
//         // mono_domain_unload(s_Data->AppDomain);
//         s_Data->AppDomain = nullptr;

//         // mono_jit_cleanup(s_Data->RootDomain);
//         s_Data->RootDomain = nullptr;
//     }
//     MonoObject *ScriptEngine::InstantiateClass(MonoClass *monoClass) {
//         // ����һ��Main�๹�ɵ�mono�����ҳ�ʼ��
//         MonoObject *instance = mono_object_new(s_Data->AppDomain, monoClass);
//         mono_runtime_object_init(instance); // ���캯�����������
//         return instance;
//     }
//     void ScriptEngine::OnRuntimeStart(Scene *scene) { s_Data->SceneContext = scene; }
//     void ScriptEngine::OnRuntimeStop() {
//         s_Data->SceneContext = nullptr;
//         s_Data->EntityInstances.clear();
//     }
//     bool ScriptEngine::EntityClassExists(const std::string &fullClassName) {
//         return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
//     }
//     void ScriptEngine::OnCreateEntity(Entity entity) {
//         const auto &sc = entity.GetComponent<ScriptComponent>(); // �õ����ʵ������
//         if (ScriptEngine::EntityClassExists(sc.ClassName)) {     // ����Ľű������Ƿ���ȷ
//             Ref<ScriptInstance> instance =
//                 CreateRef<ScriptInstance>(s_Data->EntityClasses[sc.ClassName], entity); // ʵ���������
//             s_Data->EntityInstances[entity.GetUUID()] = instance; // ���нű�map�洢��ЩScriptInstance(�����)
//             instance->InvokeOncreate();                           // ����C#��OnCreate����
//         }
//     }
//     void ScriptEngine::OnUpdateEntity(Entity entity, Timestep ts) {
//         UUID entityUUID = entity.GetUUID(); // �õ����ʵ���UUID
//         MND_CORE_ASSERT((s_Data->EntityInstances.find(entityUUID) != s_Data->EntityInstances.end()));

//         // ����UUID��ȡ��ScriptInstance��ָ��
//         Ref<ScriptInstance> instance = s_Data->EntityInstances[entityUUID];
//         instance->InvokeOnUpdate((float)ts); // ����C#��OnUpdate����
//     }
//     void ScriptEngine::LoadAssemblyClasses() {
//         s_Data->EntityClasses.clear();

//         const MonoTableInfo *typeDefinitionsTable =
//             mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
//         int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
//         // 1.����Entity����
//         MonoClass *entityClass = mono_class_from_name(s_Data->CoreAssemblyImage, "Hazel", "Entity");

//         for (int32_t i = 0; i < numTypes; i++) {
//             uint32_t cols[MONO_TYPEDEF_SIZE];
//             mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

//             const char *nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
//             const char *className = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
//             std::string fullName;
//             if (strlen(nameSpace) != 0) {
//                 fullName = fmt::format("{}.{}", nameSpace, className);
//             } else {
//                 fullName = className;
//             }
//             // 2.����Dll������C#��
//             MonoClass *monoClass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, className);
//             if (monoClass == entityClass) { // entity���಻����
//                 continue;
//             }
//             // 3.�жϵ�ǰ���Ƿ�ΪEntity������-���Ƿ�����ʵ��Ľű���
//             bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false); // ���c#���Ƿ�Ϊentity������
//             if (!isEntity) {
//                 continue;
//             }
//             // 3.1�Ǿʹ���ű�map��
//             Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(nameSpace, className);
//             s_Data->EntityClasses[fullName] = scriptClass;

//             // 123����ȡ�ű��������
//             /*
//             -
//             �ڼ���dll�󣬶�ȡ��Ϸ�ű��������������C#��õ�MonoClass����
//             - �ٸ���MonoClass����õ�C#�����������
//             -
//             ѭ�����ԣ��õ�����MonoClassField���󣬸���MonoClassField�����C#���Ե����ơ�����Ȩ�ޡ�����
//             - ����Ȩ�޾���map�Ƿ�洢�������
//             */
//             int fieldCount = mono_class_num_fields(monoClass);
//             MND_CORE_WARN("{} has {} fields:", className, fieldCount);
//             void *iterator = nullptr;
//             while (MonoClassField *field = mono_class_get_fields(monoClass, &iterator)) {
//                 const char *filedName = mono_field_get_name(field);
//                 uint32_t flags = mono_field_get_flags(field);
//                /*  if (flags & FIELD_ATTRIBUTE_PUBLIC) { // &��λ�� 1 1=1��1 0 = 0,0 1 = 0
//                     MonoType *type = mono_field_get_type(field);
//                     ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
//                     MND_CORE_TRACE("	{}({})", filedName, Utils::ScriptFieldTypeToStirng(fieldType));
//                     // ��Map�洢�������
//                     scriptClass->m_Fields[filedName] = { fieldType, filedName, field };
//                 } */
//             }
//         }
//     }
//     Scene *ScriptEngine::GetSceneContext() { return s_Data->SceneContext; }
//     MonoImage *ScriptEngine::GetCoreAssemblyImage() { return s_Data->CoreAssemblyImage; }
//     Ref<ScriptInstance> ScriptEngine::GetEntityScriptInstance(UUID entityID) {
//         auto it = s_Data->EntityInstances.find(entityID);
//         if (it == s_Data->EntityInstances.end()) {
//             return nullptr;
//         }
//         return it->second;
//     }
//     //////////////////////////////////////////////////////////////
//     // ScriptInstance/////////////////////////////////////////////
//     //////////////////////////////////////////////////////////////
//     ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity) : m_ScriptClass(scriptClass) {
//         // ��ȡSandbox Player�๹�ɵ�MonoObject�����൱��new Sandbox.Player()
//         m_Instance = scriptClass->Instantiate();

//         m_Constructor = s_Data->EntityClass.GetMethod(".ctor", 1); // ��ȡC#Entity��Ĺ��캯��
//         m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0); // ��ȡ���洢Sandbox.Player��ĺ���
//         m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 1);
//         // ����C#Entity��Ĺ��캯��
//         {
//             UUID entityID = entity.GetUUID();
//             void *param = &entityID;
//             m_ScriptClass->InvokeMethod(m_Instance, m_Constructor,
//                                         &param); // ��һ�������������Entity����(Player)���ɵ�mono����
//         }
//     }
//     void ScriptInstance::InvokeOncreate() {
//         if (m_OnCreateMethod) {
//             m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod); // ����Sandbox.Player���OnUpdate����
//         }
//     }
//     void ScriptInstance::InvokeOnUpdate(float ts) {
//         if (m_OnUpdateMethod) {
//             void *param = &ts;
//             m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
//         }
//     }
//     bool ScriptInstance::GetFieldValueInternal(const std::string &name, void *buffer) {
//         const auto &fields = m_ScriptClass->GetFields();
//         auto it = fields.find(name);
//         if (it == fields.end()) {
//             return false;
//         }
//         const ScriptField &field = it->second;
//         mono_field_get_value(m_Instance, field.ClassField, buffer);
//         return true;
//     }
//     bool ScriptInstance::SetFieldValueInternal(const std::string &name, void *value) {
//         const auto &fields = m_ScriptClass->GetFields();
//         auto it = fields.find(name);
//         if (it == fields.end()) {
//             return false;
//         }
//         const ScriptField &field = it->second;
//         mono_field_set_value(m_Instance, field.ClassField, (void *)value);
//         return true;
//     }
//     //////////////////////////////////////////////////////////////
//     // ScriptClass////////////////////////////////////////////////
//     //////////////////////////////////////////////////////////////
//     ScriptClass::ScriptClass(const std::string &classNamespace, const std::string &className, bool isCore) {
//         m_MonoClass = mono_class_from_name(isCore ? s_Data->CoreAssemblyImage : s_Data->AppAssemblyImage,
//                                            classNamespace.c_str(), className.c_str());
//     }
//     MonoObject *ScriptClass::Instantiate() { return ScriptEngine::InstantiateClass(m_MonoClass); }
//     MonoMethod *ScriptClass::GetMethod(const std::string &name, int parameterCount) {
//         return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
//     }
//     MonoObject *ScriptClass::InvokeMethod(MonoObject *instance, MonoMethod *method, void **params) {
//         return mono_runtime_invoke(method, instance, params, nullptr); // **���ͣ�&params(ʵ��) = params��ʵ�Σ�
//     }
