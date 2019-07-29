#include "XXpath.h"

XXpath::XXpath() {
}
XXpath::XXpath(const char *path)
	: _path(path)
{}
XXpath::XXpath(const string &path)
	: _path(path)
{}
XXpath::XXpath(const XXpath &xxpath) {
	*this = xxpath;
}
XXpath::~XXpath() {

}

void XXpath::operator=(const XXpath &xxpath) {
	_path = xxpath._path;
}
XXpath& XXpath::operator<<(const char *node) {
	_path += '/';
	_path += node;
	return *this;
}
XXpath& XXpath::operator<<(const string &node) {
	_path += '/';
	_path += node;
	return *this;
}
XXpath& XXpath::operator<<(int node) {
	_path += '/';
	_path += to_string(node);
	return *this;
}
