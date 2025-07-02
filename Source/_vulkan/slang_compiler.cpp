#include "_vulkan/slang_compiler.hpp"

#include "utils/file.hpp"

#include "plog/Log.h"

#include <iostream>

namespace dg
{

  SlangCompiler::SlangCompiler(const std::string& filePath)
  {
    Slang::ComPtr<slang::IGlobalSession> globalSession;
    slang::createGlobalSession(globalSession.writeRef());

    slang::TargetDesc targetDesc = {};
    targetDesc.format = SLANG_SPIRV;
    targetDesc.profile = globalSession->findProfile("spirv_1_6");

    slang::SessionDesc sessionDesc = {};
    sessionDesc.targets = &targetDesc;
    sessionDesc.targetCount = 1;

    std::array<slang::CompilerOptionEntry, 1> options = 
        {
            {
                slang::CompilerOptionName::EmitSpirvDirectly,
                {slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}
            }
        };
    sessionDesc.compilerOptionEntries = options.data();
    sessionDesc.compilerOptionEntryCount = options.size();

    globalSession->createSession(sessionDesc, m_session.writeRef());

    std::vector<char> code = file::read<char>(filePath);
    std::string codeString(code.begin(), code.end());

    Slang::ComPtr<slang::IBlob> diagnosticsBlob;
    m_module = m_session->loadModule(filePath.c_str(), diagnosticsBlob.writeRef());
    if (diagnosticsBlob != nullptr)
    {
      LOGD << (const char*)diagnosticsBlob->getBufferPointer();
    }
    if (!m_module) throw std::runtime_error("Error compiling slang shader");
  }

  ShaderModule SlangCompiler::get(const std::string& entryPointName)
  {
    Slang::ComPtr<slang::IEntryPoint> entryPoint;
    m_module->findEntryPointByName(entryPointName.c_str(), entryPoint.writeRef());
    if (!entryPoint) throw std::runtime_error("Error finding slang entry point");

    std::array<slang::IComponentType*, 2> componentTypes =
    {
      m_module,
      entryPoint
    };

    Slang::ComPtr<slang::IComponentType> composedProgram;
    {
      Slang::ComPtr<slang::IBlob> diagnosticsBlob;
      SlangResult result = m_session->createCompositeComponentType(
          componentTypes.data(),
          componentTypes.size(),
          composedProgram.writeRef(),
          diagnosticsBlob.writeRef());
      if (result != SLANG_OK) throw std::runtime_error("Error composing slang shader");
    }

    {

      Slang::ComPtr<slang::IBlob> diagnosticsBlob;
      SlangResult result = composedProgram->link(
          m_program.writeRef(),
          diagnosticsBlob.writeRef());
      if (result != SLANG_OK) throw std::runtime_error("Error linking slang shader");
     }

    ShaderModule shaderModule;
    {
      Slang::ComPtr<slang::IBlob> diagnosticsBlob;
      SlangResult result = m_program->getEntryPointCode(
          0, // entryPointIndex
          0, // targetIndex
          shaderModule.slangSpirvCode.writeRef(),
          diagnosticsBlob.writeRef());
      if (result != SLANG_OK) throw std::runtime_error("Error translating slang shader");
    }     

    shaderModule.source = ShaderSource::slang;
    return shaderModule;
  }

  void SlangCompiler::reflect(ShaderDescription& description)
  {
    slang::ProgramLayout* programLayout = m_program->getLayout();

    auto varLayout = programLayout->getGlobalParamsVarLayout();
    auto typeLayout = varLayout->getTypeLayout();
    
    unsigned int paramCount = typeLayout->getFieldCount();
    for (unsigned int i = 0 ; i < paramCount ; i++)
    {
      slang::VariableLayoutReflection* param = typeLayout->getFieldByIndex(i);
      int set = param->getBindingSpace();
      int binding = param->getBindingIndex();
      std::string name = std::string(param->getName());
      slang::BindingType type = typeLayout->getBindingRangeType(i);
      size_t arraySize = param->getType()->getElementCount();
      if (arraySize == 0) arraySize = 1;

      if (param->getCategory() == slang::ParameterCategory::DescriptorTableSlot)
      {
        size_t uboIndex = 0;
        if (param->getType()->getKind() == slang::TypeReflection::Kind::ConstantBuffer)
        {
          uboIndex = addUBO(description,
              param->getTypeLayout()->getElementTypeLayout());
        }

        description.addDescriptorSlotFromSlang(name, set, binding, type,
            arraySize, uboIndex);
      }
    }

  }

  size_t SlangCompiler::addUBO(ShaderDescription& description,
      slang::TypeLayoutReflection* typeLayout)
  {
    unsigned int paramCount = typeLayout->getFieldCount();
    UniformBuffer ubo(paramCount);

    for (unsigned int i = 0 ; i < paramCount ; i++)
    {
      slang::VariableLayoutReflection* param = typeLayout->getFieldByIndex(i);
      std::string name = std::string(param->getName());
      size_t size = typeLayout->getSize(param->getCategory());
      size_t offset = param->getOffset();

      ShaderVariableDescription variable {std::string(name),
        static_cast<uint32_t>(size), static_cast<uint32_t>(offset)};
      ubo[i] = std::move(variable);
    }

    return description.addUBO(ubo);
  }

}
