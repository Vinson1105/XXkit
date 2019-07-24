//
//  XXlevelPath.hpp
//  XXkitDemo
//
//  Created by VINSON on 2019/7/15.
//  Copyright © 2019 VINSON. All rights reserved.
//

#ifndef XXpath_h
#define XXpath_h

#include <stdio.h>
#include <string>

using namespace std;
class XXpath {
public:
	XXpath();
	XXpath(const char *path);
	XXpath(const string &path);
	XXpath(const XXpath &xxpath);
	virtual ~XXpath();

	const string& path() { return _path; }

	void operator=(const XXpath &xxpath);
	XXpath& operator<<(const char *node);
	XXpath& operator<<(const string &node);

private:
	string _path;
};

#endif /* XXlevelPath_hpp */
