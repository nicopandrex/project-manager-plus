#ifndef M3OEP_NLIPPIAT_ANALYSISBRIDGE_H
#define M3OEP_NLIPPIAT_ANALYSISBRIDGE_H

#include <filesystem>
#include <ostream>
#include <string>
#include <vector>

#include "Project.h"

std::filesystem::path getAnalysisInputPath();
std::filesystem::path getAnalysisOutputPath();

bool exportProjectsForAnalysis(const std::vector<Project> &projects,
                               const std::filesystem::path &outputPath,
                               std::string &message);

std::string buildSmartAnalysisCommand(const std::filesystem::path &inputPath,
                                      const std::filesystem::path &outputPath);

bool runSmartAnalysisPython(const std::filesystem::path &inputPath,
                            const std::filesystem::path &outputPath,
                            std::string &message);

struct AnalysisSummary
{
    int projectCount = 0;
    int taskCount = 0;
    int doneCount = 0;
    int timedTaskCount = 0;
};

struct ProjectBreakdownItem
{
    std::string project;
    int priority = 0;
    std::string status;
    int totalTasks = 0;
    int doneTasks = 0;
    double completionPercent = 0.0;
};

struct RecommendationItem
{
    std::string project;
    std::string task;
    std::string reason;
    int score = 0;
};

struct AnalysisReport
{
    AnalysisSummary summary;
    std::vector<ProjectBreakdownItem> projectBreakdown;
    std::vector<RecommendationItem> recommendations;
};

bool loadAnalysisReport(const std::filesystem::path &outputPath,
                        AnalysisReport &report,
                        std::string &message);

void printAnalysisReport(const AnalysisReport &report, std::ostream &out);

#endif // M3OEP_NLIPPIAT_ANALYSISBRIDGE_H
