#include <iostream>
#include <string>
#include <cstring>

#include "Publisher.h"

void usage(const char* invocation)
{
	std::cout << invocation << " " << std::endl;
}

struct Args
{
	std::string input;
	std::string config;
	std::string kit;
	bool singleFile = false;
	bool binaryAssets = true;
	bool help = false;
};

int main(int argc, char** argv)
{
	Args args;

	if(argc == 1)
	{
		usage(argv[0]);
		return 0;
	}

	for(int i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "--help"))
		{
			usage(argv[0]);
			return 0;
		}
		else if(!strcmp(argv[i], "--input"))
		{
			args.input = argv[++i];
		}
		else if(!strcmp(argv[i], "--config"))
		{
			args.config = argv[++i];
		}
		else if(!strcmp(argv[i], "--kit"))
		{
			args.kit = argv[++i];
		}
	}

	if(args.input.empty())
	{
		std::cout << "There was no input project directory given!" << std::endl;
		return 1;
	}

	if(args.config.empty())
		args.config = args.input + "/publish.json";

	try
	{
		Neo::Publisher pub;
		pub.loadConfig(args.config);
		pub.publish(args.input);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Could not publish project: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
