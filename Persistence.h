//
// Created by Nico on 2/16/2026.
//

#ifndef M3OEP_NLIPPIAT_PERSISTENCE_H
#define M3OEP_NLIPPIAT_PERSISTENCE_H

#include <string>
#include <vector>
#include <filesystem>
#include "Project.h"

std::string projectStatusToString(ProjectStatus status);
std::string taskStateToString(TaskState state);

std::filesystem::path getDefaultSavePath();

bool saveProjects(const std::filesystem::path &path, const std::vector<Project> &projects, std::string &message);

bool loadProjects(const std::filesystem::path &path, std::vector<Project> &outProjects, std::string &message);

#endif // M3OEP_NLIPPIAT_PERSISTENCE_H
