#include "_vulkan/slang_compiler.hpp"

#include "utils/file.hpp"

#include "plog/Log.h"

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

    Slang::ComPtr<slang::IComponentType> linkedProgram;
    {

      Slang::ComPtr<slang::IBlob> diagnosticsBlob;
      SlangResult result = composedProgram->link(
          linkedProgram.writeRef(),
          diagnosticsBlob.writeRef());
      if (result != SLANG_OK) throw std::runtime_error("Error linking slang shader");
     }

    ShaderModule shaderModule;
    {
      Slang::ComPtr<slang::IBlob> diagnosticsBlob;
      SlangResult result = linkedProgram->getEntryPointCode(
          0, // entryPointIndex
          0, // targetIndex
          shaderModule.slangSpirvCode.writeRef(),
          diagnosticsBlob.writeRef());
      if (result != SLANG_OK) throw std::runtime_error("Error translating slang shader");
    }     

    shaderModule.source = ShaderSource::slang;
    return shaderModule;
  }

}
