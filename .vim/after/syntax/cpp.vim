syn match glEnum /\v\CGL_\w+/
syn match glmVec /\v\C<vec\d>/
syn match glmMat /\v\C<mat\d>/
syn match glmNamespace /\v\C<glm>/
syn match glfwMacro /\v\C<GLFW_\w+/
syn match glfwType /\v\C<GLFW\l+/
syn match classMember /\v\C<m\u\w*>/

syn match mwNamespace /\v\C<MW\u+\l+\w*>/
syn match csNamespace /\v\C<CS\u+\l+\w*>/
" Cell Container Dialogue Sound are not included
syn keyword mwNamespaces Ai AiSequence Animation Bsa Columns Compiler Config Console ContentSelectorModel ContentSelectorView Control Controllers CS DefaultGmsts DragDropUtils ESM ESMTerrain EsmTool ESSImport Fallback Files Formatting Generator Gui HyperTextParser ICS Interpreter Launcher Loading Misc MyGUIPlugin Nif NifBullet NifOsg OIS OMW Process Resource ResourceHelpers SceneUtil ScriptTest SDLUtil Settings SFO Shader Sky Stats Terrain ToUTF8 Transformation Translation User Version VFS Video Widgets Wizard CS CSMDoc CSMFilter CSMPrefs CSMTools CSMWorld CSVDoc CSVFilter CSVPrefs CSVRender CSVTools CSVWidget CSVWorld MWBase MWClass MWDialogue MWGui MWInput MWMechanics MWPhysics MWRender MWScript MWSound MWState MWWorld
syn match mwClass /\v\C<\u\w*\l\w*>/
syn keyword mwClasses NPC NAME NAME32 NAME64 NAME256

syn keyword osgNamespaces osg osgAnimation osgDB osgFX osgGA osgManipulator osgParticle osgPresentation osgQt osgShadow osgSim osgTerrain osgText osgUI osgUtil osgViewer osgVolume osgWidget

syn match osgSmartPointer /\v\C<ref_ptr>/

syn match sdlType /\v\C<SDL_\w*[a-z]+\w*>/
syn match sdlConstant /\v\C<SDL_[A-Z_]+>/

syn keyword myguiNamespace MyGUI

syn keyword cQues Ques contained
syn cluster cCommentGroup add=cQues

highlight link glEnum Constant
highlight link glmVec Type
highlight link glmMat Type
highlight link glmNamespace Constant
highlight link glfwMacro Constant
highlight link glfwType Type

"highlight link classMember Identifier

highlight link osgNamespaces Constant
highlight link osgSmartPointer Type

highlight link mwClass Type
highlight link mwClasses Type
highlight link mwNamespaces Constant
highlight link csNamespace Constant

highlight link sdlType Type
highlight link sdlConstant Constant

highlight link myguiNamespace Constant

highlight link cQues Todo
