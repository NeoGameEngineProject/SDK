#ifndef NEO_VRCONTEXT_H
#define NEO_VRCONTEXT_H

#include <openvr.h>
#include <Renderer.h>
#include <Level.h>
#include <InputContext.h>

namespace Neo
{

// TODO: NeoHTML integration!
class VRContext
{
	vr::IVRSystem* m_system = nullptr;
	vr::TrackedDevicePose_t m_poses[vr::k_unMaxTrackedDeviceCount];
	
	unsigned int m_leftFbo, m_rightFbo, m_leftDepth, m_rightDepth, m_leftTexture, m_rightTexture, 
			m_leftResolveFbo, m_rightResolveFbo, 
			m_leftResolveTexture, m_rightResolveTexture;
	
	unsigned int m_width, m_height, m_camWidth, m_camHeight;
	
	bool setupFBO(unsigned int w, unsigned int h, unsigned int& fbo, unsigned int& depth, unsigned int& renderTexture, unsigned int& resolveFbo, unsigned int& resolveTexture);
	void reportCompositorError(vr::VRCompositorError err);
	
	/**
	 * @brief Models a Vive controller.
	 * 
	 * 16 Axis: (X, Y, Z) position, (X, Y, Z) rotation, +2*5 for other axis
	 */
	class ViveInputDevice : public FixedAxisInputDevice<16>
	{
	public:
		// 5 Buttons
		ViveInputDevice(): FixedAxisInputDevice<16>(vr::k_EButton_Max) {}
		void setPose(const vr::TrackedDevicePose_t& pose, const vr::VRControllerState_t& state);
	};
	
	std::array<ViveInputDevice, vr::k_unMaxTrackedDeviceCount> m_inputDevices;
	AxisInputDevice* m_leftHand = nullptr, *m_rightHand = nullptr;
public:
	
	enum BUTTONS
	{
		SYSTEM_BTN = vr::k_EButton_System,
		APPMENU_BTN = vr::k_EButton_ApplicationMenu,
		GRIP_BTN = vr::k_EButton_Grip,
		DPAD_LEFT_BTN = vr::k_EButton_DPad_Left,
		DPAD_UP_BTN = vr::k_EButton_DPad_Up,
		DPAD_RIGHT_BTN = vr::k_EButton_DPad_Right,
		DPAD_DOWN_BTN = vr::k_EButton_DPad_Down,
		A_BTN = vr::k_EButton_A,
		PROXIMITY_SENSOR = vr::k_EButton_ProximitySensor,
		AXIS0_BTN = vr::k_EButton_Axis0,
		AXIS1_BTN = vr::k_EButton_Axis1,
		AXIS2_BTN = vr::k_EButton_Axis2,
		AXIS3_BTN = vr::k_EButton_Axis3,
		AXIS4_BTN = vr::k_EButton_Axis4,
	};
	
	enum AXIS
	{
		AXIS0_X = 6,
		AXIS0_Y = 7,
		AXIS1_X = 8,
		AXIS1_Y = 9,
		AXIS2_X = 10,
		AXIS2_Y = 11,
		AXIS3_X = 12,
		AXIS3_Y = 13,
		AXIS4_X = 14,
		AXIS4_Y = 15
	};
	
	enum EYE
	{
		LEFT_EYE = vr::Eye_Left,
		RIGHT_EYE = vr::Eye_Right
	};
	
	~VRContext();
	bool initialize();
	
	void draw(Renderer& render, Level& level);
	
	void update();
	void enableEye(CameraBehavior& camera, EYE eye);
	void endDraw(CameraBehavior& camera);
	
	AxisInputDevice& getInputDevice(size_t id) { assert(id >= 0 && id < m_inputDevices.size()); return m_inputDevices[id]; }
	
	AxisInputDevice* getLeftHand() { return m_leftHand; }
	AxisInputDevice* getRightHand() { return m_rightHand; }
	
	unsigned int getWidth() const { return m_width; }
	unsigned int getHeight() const { return m_height; }
	
	bool hasVR() const { return m_system != nullptr; }
};

}

#endif // NEO_VRCONTEXT_H
