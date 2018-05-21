#include "FileTask.hpp"

FileTask::FileTask(string str) : Task(str) {}

FileTask::~FileTask() {}

string FileTask::getName()
{
    return name;
}
