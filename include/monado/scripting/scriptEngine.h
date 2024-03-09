#pragma once

#include "monado/scene/scene.h"
#include "monado/scene/entity.h"

#include "monado/core/core.h"
#include "monado/core/uuid.h"

#include <filesystem>
#include <string>
#include <map>

// ���������ͷ�ļ��������ⲿ����������Щ������c�ļ�����Ľṹ��������Ҫextern"C"
extern "C" {
typedef struct _MonoClass MonoClass;
typedef struct _MonoObject MonoObject;
typedef struct _MonoMethod MonoMethod;
typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoImage MonoImage;
typedef struct _MonoClassField MonoClassField;
}

namespace Monado {

    enum class ScriptFieldType {
        None = 0,
        Float,
        Double,
        Bool,
        Char,
        Byte,
        Short,
        Int,
        Long,
        UByte,
        UShort,
        UInt,
        ULong,
        Vector2,
        Vector3,
        Vector4,
        Entity
    };
    // �������ƶ�Ӧ�Ľṹ��
    struct ScriptField {
        ScriptFieldType Type;
        std::string Name;

        MonoClassField *ClassField;
    };
    // �൱�ڴ���Class����װ��Class�����ԣ����ĸ������ռ���������ش����ģ��洢������Class����Ϊ��ʵ����Class�ࡢ��ȡ��ĺ�����������ĺ���
    class ScriptClass {
    public:
        ScriptClass() = default;
        ScriptClass(const std::string &classNamespace, const std::string &className,
                    bool isCore = false); // 119.3. ����һ��MonoClass��

        MonoObject *
        Instantiate(); // 119.4.����һ����MonoClass�๹�ɵ�mono�����ҳ�ʼ��
        MonoMethod *GetMethod(const std::string &name, int parameterCount); // 119.5.1 ��ȡ��ĺ���
        MonoObject *InvokeMethod(MonoObject *instance, MonoMethod *method,
                                 void **params = nullptr); // 119.5.2 ������ĺ���
        // 123.����
        const std::map<std::string, ScriptField> &GetFields() const { return m_Fields; }

    private:
        std::string m_ClassNamespace;
        std::string m_ClassName;
        MonoClass *m_MonoClass = nullptr;

        std::map<std::string, ScriptField> m_Fields;

        friend class ScriptEngine;
    };

    // 120.C#��ʵ�彻��
    // �൱��new Class();�õ��Ķ����ٷ�װһ�㣬���ԣ����ĸ�Class��ʼ�����洢Class�ĺ�������Ϊ������Class�ĺ���
    class ScriptInstance {
    public:
        ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);

        void InvokeOncreate();         // ����C#���OnCreate����
        void InvokeOnUpdate(float ts); // ����C#���OnUpdate����
        // 123.����
        Ref<ScriptClass> GetScriptClass() { return m_ScriptClass; }

        template <typename T>
        T GetFieldValue(const std::string &name) {
            bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
            if (!success)
                return T();
            return *(T *)s_FieldValueBuffer;
        }
        template <typename T>
        void SetFieldValue(const std::string &name, T &value) {
            SetFieldValueInternal(name, &value); // ���ñ����ĵ�ַ ���� �����ñ����ĵ�ַ
        }

    private:
        bool GetFieldValueInternal(const std::string &name, void *buffer);
        bool SetFieldValueInternal(const std::string &name, void *value);

    private:
        inline static char s_FieldValueBuffer[8];

        Ref<ScriptClass> m_ScriptClass;

        MonoObject *m_Instance = nullptr;
        MonoMethod *m_Constructor = nullptr;
        MonoMethod *m_OnCreateMethod = nullptr;
        MonoMethod *m_OnUpdateMethod = nullptr;
    };

    // ��Ҫ�Ǽ��س�ʼ��mono��dll���Լ����ⲿ�ṩ����
    class ScriptEngine {
    public:
        static void Init();     // ��ʼ��
        static void Shutdown(); // �ر�

        static void LoadAssembly(const std::filesystem::path &filepath);    // 119.2.����dll����
        static void LoadAppAssembly(const std::filesystem::path &filepath); // 119.2.����dll����

        // 120.C#��ʵ�彻��
        static void OnRuntimeStart(Scene *scene); // �������п�ʼʱ
        static void OnRuntimeStop();              // �������н���ʱ

        static bool EntityClassExists(const std::string &fullClassName); // ����Ƿ���ڴ˿ռ�+������ʵ��
        static void OnCreateEntity(Entity entity);                       // �������п�ʼʱִ��
        static void OnUpdateEntity(Entity entity, Timestep ts); // ��������ʱ

        static Scene *GetSceneContext();
        // 121.���
        static MonoImage *GetCoreAssemblyImage();
        // 123.����
        // ����ʵ���uuid����ȡ��Ӧ��ScriptInstance��
        static Ref<ScriptInstance> GetEntityScriptInstance(UUID entityID);

    private:
        static void InitMono();     // 119.1.��ʼ��mono
        static void ShutdownMono(); // �ر�mono

        static MonoObject *InstantiateClass(
            MonoClass
                *monoClass); // 119.4.����һ����MonoClass�๹�ɵ�mono�����ҳ�ʼ��
        friend class ScriptClass;

        // 120.C#��ʵ�彻��
        static void LoadAssemblyClasses(); // ����C#�и�����entity���࣬
    };
} // namespace Monado
