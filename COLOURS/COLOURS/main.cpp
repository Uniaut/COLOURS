#include "Game2D.h"
#include <random>
#include "fmod.hpp"

namespace shapes
{
	using namespace jm;
	class wiredCube
	{
	public:
		RGB color;
		float size;
		float rotation;
		float cam;

		wiredCube()
			:color(Colors::black)
		{}
		wiredCube(const RGB &_color, const float &_size = 1.0f, const float &_rotation = 1.0f, const float &_cam = 1.0f)
			:color(_color), size(_size), rotation(_rotation), cam(_cam)
		{
		}

		wiredCube& init(const RGB &_color, const float &_size = 1.0f, const float &_rotation = 1.0f, const float &_cam = 1.0f)
		{
			color = _color;
			size = _size;
			rotation = _rotation;
			cam = _cam;

			return (*this);
		}

		wiredCube& setRotation(const float &_rotation)
		{
			rotation = _rotation;
			return (*this);
		}

		void drawColLine(const float &x, const float &y)
		{
			beginTransformation();
			{
				translate(x, y);
				rotate(-rotation);
				scale(1.0f, 1.0f / sinf(getRadian(cam)));
				drawLine(color, vec2(0.0f, 0.0f), color, vec2(0.0, -size * cosf(getRadian(cam))));
			}
			endTransformation();
		}

		void draw()
		{
			beginTransformation();
			{
				translate(0.0f, size * cosf(getRadian(cam)) / 2.0f);
				scale(1.0f, sinf(getRadian(cam)));
				rotate(rotation);
				drawWiredBox(color, size, size);

				drawColLine(size / 2.0f, size / 2.0f);
				drawColLine(-size / 2.0f, size / 2.0f);
				drawColLine(size / 2.0f, -size / 2.0f);
				drawColLine(-size / 2.0f, -size / 2.0f);
			}
			endTransformation();
			beginTransformation();
			{
				translate(0.0f, -size * cosf(getRadian(cam)) / 2.0f);
				scale(1.0f, sinf(getRadian(cam)));
				rotate(rotation);
				drawWiredBox(color, size, size);
			}
			endTransformation();
		}

	};

	//
	class physicsCircle
	{
	private:
		RGB color;
		vec2 position;
		vec2 velocity;
		float size;
		float gravity;

		bool colision = false;
	public:
		bool mouseOn(vec2 pos)
		{
			float dist2 = (position - pos).x * (position - pos).x + (position - pos).y * (position - pos).y;
			if (dist2 < size * size)
				return true;
			else
				return false;

		}

		bool ifColision()
		{
			return colision;
		}

		vec2 getPos()
		{
			return position;
		}
		vec2 getVel()
		{
			return velocity;
		}

		RGB getColor()
		{
			return color;
		}

		physicsCircle(const RGB & _color, const float _size = 1.0f, vec2 _pos = vec2(0.0f, 0.0f), vec2 _vel = vec2(0.0f, 0.0f), float _g = 1.0f)
			:color(_color), size(_size), position(_pos), velocity(_vel), gravity(_g)
		{}

		physicsCircle& set(const RGB & _color, const float _size = 1.0f, vec2 _pos = vec2(0.0f, 0.0f), vec2 _vel = vec2(0.0f, 0.0f), float _g = 1.0f)
		{
			color = _color;
			size = _size;
			position = _pos;
			velocity = _vel;
			gravity = _g;

			return (*this);
		}

		physicsCircle& setG(const float _g)
		{
			gravity = _g;
			return (*this);

		}

		physicsCircle& setColor(const RGB & _color)
		{
			color = _color;
			return (*this);

		}

		physicsCircle& setPos(const vec2 & _pos)
		{
			position = _pos;
			return (*this);

		}

		physicsCircle& update(float deltaTime)
		{
			//충돌판정
			colision = false;

			velocity.y -= gravity * deltaTime;
			position += velocity * deltaTime;


			if (position.y <= -0.3f)
			{
				colision = true;

				velocity = -velocity;
				position += velocity * deltaTime;
			}

			//if (velocity.y <= 0.05f && velocity.y >= -0.05f)
			//std::clog << "data : " << gravity << std::endl;

			return (*this);
		}

		physicsCircle& draw()
		{
			beginTransformation();
			{
				translate(position);
				drawFilledCircle(color, 0.1f);
				RGB temp(color.r * 0.8f, color.g * 0.8f, color.b * 0.8f);
				drawWiredCircle(temp, 0.1f);
			}
			endTransformation();

			return (*this);
		}
	};
}

namespace sound
{
	class SOUND
	{
		FMOD::System     *system;
		FMOD::Sound      *sound[4];
		FMOD::Channel    *channel;
		FMOD::Channel    *gochannel;
		FMOD_RESULT       result;
		void             *extradriverdata;

	public:
		SOUND()
			:system(nullptr), channel(nullptr), extradriverdata(nullptr)
		{
			sound[0] = nullptr;
			sound[1] = nullptr;
			sound[2] = nullptr;
			sound[3] = nullptr;

			result = FMOD::System_Create(&system);
			result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);

			result = system->createSound("sound/A.mp3", FMOD_LOOP_OFF, 0, &sound[0]); // .mp3 files work!
			if (result != FMOD_OK) std::cerr << "read fail." << std::endl;
			result = system->createSound("sound/B.mp3", FMOD_LOOP_OFF, 0, &sound[1]); // .mp3 files work!
			if (result != FMOD_OK) std::cerr << "read fail." << std::endl;
			result = system->createSound("sound/C.mp3", FMOD_LOOP_OFF, 0, &sound[2]); // .mp3 files work!
			if (result != FMOD_OK) std::cerr << "read fail." << std::endl;
			result = system->createSound("sound/Gameover.mp3", FMOD_LOOP_OFF, 0, &sound[3]); // .mp3 files work!
			if (result != FMOD_OK) std::cerr << "read fail." << std::endl;

		}

		~SOUND()
		{
			system->release();
		}

		void playA()
		{
			bool playing = false;
			channel->isPlaying(&playing);

			if (playing)channel->setPaused(true);

			result = system->playSound(sound[0], 0, false, &channel);
		}
		void playB()
		{
			bool playing = false;
			channel->isPlaying(&playing);

			if (playing)channel->setPaused(true);
			result = system->playSound(sound[1], 0, false, &channel);
		}
		void playC()
		{
			bool playing = false;
			channel->isPlaying(&playing);

			if (playing)channel->setPaused(true);
			result = system->playSound(sound[2], 0, false, &channel);
		}
		void playGO()
		{
			bool playing = false;
			channel->isPlaying(&playing);
			if (playing)channel->setPaused(true);

			result = system->playSound(sound[3], 0, false, &gochannel);
		}
	};


}

namespace jm
{
	int getRandomNumber(int min, int max)
	{
		std::random_device rn;
		std::mt19937_64 rnd(rn());
		std::uniform_int_distribution<int> range(min, max);

		return range(rnd);
	}

	class COLOUR : public Game2D
	{
	private:
		shapes::wiredCube *cubes;
		shapes::physicsCircle *pcircle;

		int ballID = 0;
		int boxID = 0;// 맨앞 박스 번호
		float angle = 0.0f; // 실제 각도.

		const float animeTime = 0.1f; // 돌릴때 애니메이션
		int animeCnt = 0;
		bool leftAnimated = false;
		bool rightAnimated = false;


		bool start = false;
		bool gameover = false;

		int score = 0;

		bool goSoundPlayed = false;
		sound::SOUND snd;
		vec2 prevVel;
		vec2 nowVel;

		Timer playtime;

	public:
		COLOUR()
			:Game2D("COLOUR! click ball to start / restart, left & right arrow key to control", 1280, 960, false)
		{
			cubes = new shapes::wiredCube[6];
			cubes[0].init(Colors::pastelRed, 0.3f, 0.0f, 30.0f);
			cubes[1].init(Colors::pastelYellow, 0.3f, 0.0f, 30.0f);
			cubes[2].init(Colors::pastelGreen, 0.3f, 0.0f, 30.0f);
			cubes[3].init(Colors::pastelCyan, 0.3f, 0.0f, 30.0f);
			cubes[4].init(Colors::pastelBlue, 0.3f, 0.0f, 30.0f);
			cubes[5].init(Colors::pastelMagenta, 0.3f, 0.0f, 30.0f);

			pcircle = new shapes::physicsCircle(Colors::gray, 0.1f, vec2(0.0f, 0.2f), vec2(0.0f, 0.0f), 1.8f);

			start = true;

			angle = -90.0f;

			prevVel = vec2(0.0f, 0.0f);
			nowVel = vec2(0.0f, 0.0f);

		}

		~COLOUR()
		{
			delete[] cubes;
			delete pcircle;
		}


		void update()
		{
			//프린트를 위한 준비 - 게임중
			if (pcircle->ifColision())
			{
				if (pcircle->getColor().r == Colors::gray.r
					&& pcircle->getColor().g == Colors::gray.g
					&& pcircle->getColor().b == Colors::gray.b)// 그니까 회색이면 게임 시작이란 뜻이다.
				{
					snd.playA();
					ballID = getRandomNumber(0, 5);
					pcircle->setColor(cubes[ballID].color);
				}
				else if (ballID == boxID) // 색깔이 같으면 
				{
					score++;


					ballID = getRandomNumber(0, 5);
					pcircle->setColor(cubes[ballID].color);
					pcircle->setG(1.8f + log10f((float)score));
				}
				else
				{
					playtime.reset();
					gameover = true;
					ballID = -1;
					pcircle->setColor(Colors::black);
				}

			}

			// 사운드 재생 : 꼭대기일때와 아래일때
			if (pcircle->ifColision()) // 공이 닿을때
			{
				if(!gameover)
				{
					if (score % 2 == 1) snd.playC();
					else				snd.playA();
				}
				else
				{
					if (!goSoundPlayed)
					{
						snd.playGO();
						goSoundPlayed = true;
					}
				}
			}
			//공이 꼭대기일때 (velocity가 양수에서 음수로 바뀔때)
			prevVel = nowVel;
			nowVel = pcircle->getVel();
			if (score >= 1 && prevVel.y >= 0.0f && nowVel.y < 0.0f)
				snd.playB();

			// 프린트를 위한 준비 - 객체 위치
			if (gameover)
			{
				pcircle->setPos(vec2(0.0f, -0.3f));
			}
			if (start)
			{
				pcircle->setPos(vec2(0.0f, 0.0f));
			}
			if (leftAnimated)
			{
				angle -= (60.0f / animeTime) * this->getTimeStep();
				if (animeCnt + 1 >= animeTime / this->getTimeStep()) leftAnimated = false;
				else animeCnt++;
			}
			if (rightAnimated)
			{
				angle += (60.0f / animeTime) * this->getTimeStep();
				if (animeCnt + 1 >= animeTime / this->getTimeStep()) rightAnimated = false;
				else animeCnt++;
			}

			// Scene Print
			drawFilledBox(RGB(60, 60, 60), 4.0f, 2.0f);
			drawFilledTriangle(RGB(230, 230, 230), vec2(0.0f, 0.75f), vec2(-3.5f, -1.0f), vec2(3.5f, -1.0f));

			setLineWidth(5);
			beginTransformation();
			{
				scale(1.0f, 0.5f);

				for (int i = 0; i != 6; i++)
				{
					const float temp = (float)i * 60.0f + angle;
					beginTransformation();
					rotate(temp);
					translate(1.0f, 0.0f);
					rotate(-temp);
					scale(1.0f, 2.0f);

					cubes[i].setRotation(temp).draw();
					endTransformation();
				}
			}
			endTransformation();

			if (!(start || gameover))
				pcircle->update(this->getTimeStep()).draw();
			else
				pcircle->draw();

			// 키 입력받기
			if (!(leftAnimated || rightAnimated || start || gameover))
			{
				// 왼쪽 누르면 leftAnimated true, 에니메이션 작동.
				if (isKeyPressedAndReleased(GLFW_KEY_RIGHT))
				{
					animeCnt = 0;
					leftAnimated = true;
					rightAnimated = false;

					boxID = (boxID + 1) % 6;
				}

				// 오른쪽 누르면 rightAnimated true, 에니메이션 작동.
				if (isKeyPressedAndReleased(GLFW_KEY_LEFT))
				{
					animeCnt = 0;
					leftAnimated = false;
					rightAnimated = true;

					boxID = (boxID + 5) % 6;
				}
			}
			else if (start || gameover)
			{
				if (isMouseButtonPressed(GLFW_MOUSE_BUTTON_1))
				{
					if (pcircle->mouseOn(this->getCursorPos()))
					{
						score = 0;

						pcircle->set(Colors::gray, 0.1f, vec2(0.0f, 0.2f), vec2(0.0f, 0.0f), 1.8f).update(this->getTimeStep());
						start = false; gameover = false;

						playtime.reset();
						playtime.start();

						goSoundPlayed = false;
					}
				}
			}

		}
	};

}

int main(void)
{
	jm::COLOUR().run();

	return 0;
}
