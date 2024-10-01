// 명령어			옵션		원본 파일이 있는 위치					사본 파일을 저장할 위치
//이펙트
xcopy			/y		.\EffectTool\Bin\ShaderFiles\Shader_Trail.hlsl			.\Client\Bin\ShaderFiles\Shader_Trail.hlsl	
xcopy			/y		.\EffectTool\Bin\ShaderFiles\Shader_VtxInstance_Point.hlsl	.\Client\Bin\ShaderFiles\Shader_VtxInstance_Point.hlsl
xcopy			/y		.\EffectTool\Bin\ShaderFiles\Shader_Aura.hlsl			.\Client\Bin\ShaderFiles\Shader_Aura.hlsl
xcopy			/y		.\EffectTool\Bin\ShaderFiles\Shader_VtxInstance_Mesh.hlsl			.\Client\Bin\ShaderFiles\Shader_VtxInstance_Mesh.hlsl			

xcopy			/y		.\EffectTool\Public\Effect.h					.\Client\Public\Effect.h	
xcopy			/y		.\EffectTool\Public\TRailEffect.h				.\Client\Public\TRailEffect.h	
xcopy			/y		.\EffectTool\Public\Particle_Point.h				.\Client\Public\Particle_Point.h
xcopy			/y		.\EffectTool\Public\Particle_Mesh.h				.\Client\Public\Particle_Mesh.h	
xcopy			/y		.\EffectTool\Public\Aura.h					.\Client\Public\Aura.h	

xcopy			/y		.\EffectTool\Private\Effect.cpp				.\Client\Private\Effect.cpp	
xcopy			/y		.\EffectTool\Private\TRailEffect.cpp				.\Client\Private\TRailEffect.cpp	
xcopy			/y		.\EffectTool\Private\Particle_Point.cpp			.\Client\Private\Particle_Point.cpp
xcopy			/y		.\EffectTool\Private\Particle_Mesh.cpp			.\Client\Private\Particle_Mesh.cpp
xcopy			/y		.\EffectTool\Private\Aura.cpp				.\Client\Private\Aura.cpp
