#include "VRContext.h"
#include <Log.h>

#include <GL/glew.h>

using namespace Neo;

static Matrix4x4 vrToNeo(const vr::HmdMatrix44_t& vrMat44)
{
	return Matrix4x4(
		vrMat44.m[0][0], vrMat44.m[1][0], vrMat44.m[2][0], vrMat44.m[3][0],
		vrMat44.m[0][1], vrMat44.m[1][1], vrMat44.m[2][1], vrMat44.m[3][1],
		vrMat44.m[0][2], vrMat44.m[1][2], vrMat44.m[2][2], vrMat44.m[3][2],
		vrMat44.m[0][3], vrMat44.m[1][3], vrMat44.m[2][3], vrMat44.m[3][3]
	);
}

static Matrix4x4 vrToNeo(const vr::HmdMatrix34_t& vrMat34)
{
	return Matrix4x4(
		vrMat34.m[0][0], vrMat34.m[1][0], vrMat34.m[2][0], 0.0f,
		vrMat34.m[0][1], vrMat34.m[1][1], vrMat34.m[2][1], 0.0f,
		vrMat34.m[0][2], vrMat34.m[1][2], vrMat34.m[2][2], 0.0f,
		vrMat34.m[0][3], vrMat34.m[1][3], vrMat34.m[2][3], 1.0f
	);
}

void VRContext::ViveInputDevice::setPose(const vr::TrackedDevicePose_t& pose, const vr::VRControllerState_t& state)
{
	/*
	 *  9 Axis: (X, Y, Z) position, (X, Y, Z) rotation, (X, Y) thumb position on touchpad, Z trigger
	 * 5 Buttons: 3 on top, 1 trigger, 1 side */
	Matrix4x4 mtx(vrToNeo(pose.mDeviceToAbsoluteTracking));
	
	const auto pos = mtx.getTranslationPart();
	setAxis(0, pos.x);
	setAxis(1, pos.y);
	setAxis(2, pos.z);

	const auto rot = mtx.getEulerAngles();
	setAxis(3, rot.x);
	setAxis(4, rot.y);
	setAxis(5, rot.z);
	
	
	unsigned short vrAxis = 0, i = AXIS0_X;
	assert(getAxisCount() >= i + 2*vr::k_unControllerStateAxisCount);
	for(; i < getAxisCount() && vrAxis < vr::k_unControllerStateAxisCount; vrAxis++)
	{
		setAxis(i++, state.rAxis[vrAxis].x);
		setAxis(i++, state.rAxis[vrAxis].y);
	}
}

VRContext::~VRContext()
{
	vr::VR_Shutdown();
}

bool VRContext::initialize()
{
	if(!vr::VR_IsRuntimeInstalled())
	{
		LOG_ERROR("No VR runtime found!");
		return false;
	}

	LOG_INFO("Found the VR runtime.");
	
	if(!vr::VR_IsHmdPresent())
	{
		LOG_ERROR("No HMD present!");
		return false;
	}
	
	LOG_INFO("HMD is present.");
	
	vr::HmdError error;
	m_system = vr::VR_Init(&error, vr::VRApplication_Scene);
	
	if(!m_system)
	{
		LOG_ERROR("Could not initialize HMD: " << vr::VR_GetVRInitErrorAsEnglishDescription(error));
		return false;
	}
	
	if(!vr::VRCompositor())
	{
		LOG_ERROR("Could not initialize VR compositor!");
		return false;
	}
	
	LOG_INFO("Successfully initialized OpenVR!");
	
	// Initialize graphics
	m_system->GetRecommendedRenderTargetSize(&m_width, &m_height);
	if(!setupFBO(m_width, m_height, m_leftFbo, m_leftDepth, m_leftTexture, m_leftResolveFbo, m_leftResolveTexture))
	{
		LOG_ERROR("Could not set up left eye FBO!");
		return false;
	}
	
	if(!setupFBO(m_width, m_height, m_rightFbo, m_rightDepth, m_rightTexture, m_rightResolveFbo, m_rightResolveTexture))
	{
		LOG_ERROR("Could not set up right eye FBO!");
		return false;
	}
	
	return true;
}

void VRContext::draw(Renderer& render, Level& level)
{
	auto compositor = vr::VRCompositor();
	if(!compositor->CanRenderScene())
		return;
	
	update();
	enableEye(*level.getCurrentCamera(), Neo::VRContext::LEFT_EYE);
	level.draw(render);
	
	enableEye(*level.getCurrentCamera(), Neo::VRContext::RIGHT_EYE);	
	level.draw(render);
	
	endDraw(*level.getCurrentCamera());
}

void VRContext::enableEye(CameraBehavior& camera, EYE eye)
{
	const auto vrMat43 = m_system->GetEyeToHeadTransform((vr::EVREye) eye);
	const Matrix4x4 eyeMatrix = vrToNeo(vrMat43);
	
	const auto vrMat44 = m_system->GetProjectionMatrix((vr::EVREye) eye, 0.1f, 1000.0f);
	const Matrix4x4 projMatrix = vrToNeo(vrMat44);
	
	camera.unlockPerspective();
	
	// State for cleanup in endDraw
	m_camWidth = camera.getWidth();
	m_camHeight = camera.getHeight();
	
	auto obj = camera.getParent();
	
	obj->getTransform() = eyeMatrix * obj->getTransform() * vrToNeo(m_poses[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking);
	camera.enable(m_width, m_height);
	obj->updateMatrix();
	
	camera.getProjectionMatrix() = projMatrix;
	camera.lockPerspective();
	
	// Bind GL eye
//#define MAIN_DISPLAY
#ifdef MAIN_DISPLAY
	return;
#endif
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_MULTISAMPLE);
	
	switch(eye)
	{
		case LEFT_EYE: glBindFramebuffer(GL_FRAMEBUFFER, m_leftFbo); break;
		case RIGHT_EYE: glBindFramebuffer(GL_FRAMEBUFFER, m_rightFbo); break;
	}
	
	glViewport(0, 0, m_width, m_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void VRContext::update()
{
	auto compositor = vr::VRCompositor();

	// compositor->CompositorBringToFront();
	compositor->WaitGetPoses(m_poses, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
	
	vr::VRControllerState_t ctrlState;
	for(size_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		if(!m_poses[i].bDeviceIsConnected || !m_poses[i].bPoseIsValid)
			continue;
		
		m_system->GetControllerState(i, &ctrlState, sizeof(ctrlState));
		
		auto& dev = m_inputDevices[i];
		dev.setPose(m_poses[i], ctrlState);
	}

	vr::VREvent_t event;
	while(m_system->PollNextEvent(&event, sizeof(event)))
	{
		if(event.eventAgeSeconds > 1.0f)
		{
			LOG_DEBUG("Old event: " << event.eventAgeSeconds << "s");
			continue;
		}
		
		switch(event.eventType)
		{
			case vr::VREvent_TrackedDeviceActivated:
				LOG_DEBUG("Device activated: " << event.trackedDeviceIndex);
				// ATTENTION: NO BREAK HERE! ACTIVATION SHALL DECIDE HANDS!
			case vr::VREvent_TrackedDeviceRoleChanged:
				switch(m_system->GetControllerRoleForTrackedDeviceIndex(event.trackedDeviceIndex))
				{
					case vr::TrackedControllerRole_LeftHand:
						LOG_DEBUG(event.trackedDeviceIndex << " is now the left hand!");
						m_leftHand = &m_inputDevices[event.trackedDeviceIndex];
						break;
						
					case vr::TrackedControllerRole_RightHand:
						LOG_DEBUG(event.trackedDeviceIndex << " is now the right hand!");
						m_rightHand = &m_inputDevices[event.trackedDeviceIndex];
						break;
						
					default:
						if(m_rightHand == &m_inputDevices[event.trackedDeviceIndex])
							m_rightHand = nullptr;
						else if(m_leftHand == &m_inputDevices[event.trackedDeviceIndex])
							m_leftHand = nullptr;
				};
				
				break;
				
			case vr::VREvent_TrackedDeviceDeactivated:
				LOG_DEBUG("Device deactivated: " << event.trackedDeviceIndex);
				break;
				
			case vr::VREvent_ButtonPress:
				m_inputDevices[event.trackedDeviceIndex].keyDown(event.data.controller.button);
				break;
			
			case vr::VREvent_ButtonUnpress:
				m_inputDevices[event.trackedDeviceIndex].keyUp(event.data.controller.button);
				break;
			
			default:
				continue;
		}
	}
}

void VRContext::endDraw(CameraBehavior& camera)
{
	camera.unlockPerspective(); 
	camera.enable(m_camWidth, m_camHeight);
	
	glViewport(0, 0, m_camWidth, m_camHeight);

	
#ifdef MAIN_DISPLAY
	return;
#endif
	
	glDisable(GL_MULTISAMPLE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

 	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_leftFbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_leftResolveFbo);

	glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, 
				GL_COLOR_BUFFER_BIT,
				GL_LINEAR);
	
 	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_rightFbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_rightResolveFbo);
	
	glBlitFramebuffer( 0, 0, m_width, m_height, 0, 0, m_width, m_height, 
				GL_COLOR_BUFFER_BIT,
				GL_LINEAR);
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
	vr::Texture_t leftEyeTexture = {(void*)(uintptr_t) m_leftResolveTexture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	reportCompositorError(vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture));
	
	vr::Texture_t rightEyeTexture = {(void*)(uintptr_t) m_rightResolveTexture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	reportCompositorError(vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture));
	glFlush();
	glFinish();
}

void VRContext::reportCompositorError(vr::VRCompositorError err)
{
	if(err != vr::VRCompositorError::VRCompositorError_None)
	{
		LOG_ERROR("Compositor error: " << err);
	}
}

bool VRContext::setupFBO(unsigned int w, unsigned int h, unsigned int& fbo, unsigned int& depth, unsigned int& renderTexture, unsigned int& resolveFbo, unsigned int& resolveTexture)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenRenderbuffers(1, &depth);
	glBindRenderbuffer(GL_RENDERBUFFER, depth);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, renderTexture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, w, h, true);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, renderTexture, 0);

	glGenFramebuffers(1, &resolveFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, resolveFbo);

	glGenTextures(1, &resolveTexture);
	glBindTexture(GL_TEXTURE_2D, resolveTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resolveTexture, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}






