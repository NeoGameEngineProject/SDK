#ifndef NEO_SDLINPUTCONTEXT_H
#define NEO_SDLINPUTCONTEXT_H

#include <InputContext.h>
#include <unordered_map>
#include <SDL.h>

class NEO_ENGINE_EXPORT SDLInputContext : public Neo::InputContext
{
	Neo::INPUT_KEYS translateEvent(SDL_Keycode key);

#ifndef EMSCRIPTEN
	int addGameController(int sdlId);
	int removeGameController(int index);

	class NEO_ENGINE_EXPORT SDLJoystick : public Neo::DynamicAxisInputDevice
	{
	public:
		SDLJoystick(unsigned int numberOfAxis, SDL_JoystickID id, SDL_Joystick* dev)
			: DynamicAxisInputDevice(numberOfAxis),
			  id(id),
			  device(dev)
		{ }

		SDLJoystick(unsigned int numberOfAxis, unsigned int numberOfKeys,
					SDL_JoystickID id, SDL_Joystick* dev)
			: DynamicAxisInputDevice(numberOfAxis, numberOfKeys),
			  id(id),
			  device(dev)
		{ }

		SDL_JoystickID id;
		SDL_Joystick* device;
	};

	class NEO_ENGINE_EXPORT SDLController : public Neo::DynamicAxisInputDevice
	{
	public:
		SDLController(unsigned int numberOfAxis, SDL_JoystickID id, SDL_GameController* dev)
			: DynamicAxisInputDevice(numberOfAxis),
			  id(id),
			  device(dev)
		{ }

		SDLController(unsigned int numberOfAxis, unsigned int numberOfKeys,
					SDL_JoystickID id, SDL_GameController* dev)
			: DynamicAxisInputDevice(numberOfAxis, numberOfKeys),
			  id(id),
			  device(dev)
		{ }

		SDL_JoystickID id;
		SDL_GameController* device;
	};

	std::unordered_map<SDL_JoystickID, std::shared_ptr<Neo::AxisInputDevice>> m_devices;
#endif

public:
	virtual void handleInput();
	virtual void setMouseRelative(bool value);
};

#endif //NEO_SDLINPUTCONTEXT_H
