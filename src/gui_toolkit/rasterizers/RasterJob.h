/*
 *   AviTab - Aviator's Virtual Tablet
 *   Copyright (C) 2018 Folke Will <folko@solhost.org>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef SRC_GUI_TOOLKIT_RASTERIZERS_RASTERJOB_H_
#define SRC_GUI_TOOLKIT_RASTERIZERS_RASTERJOB_H_

#include <string>
#include <future>
#include <mupdf/fitz.h>

namespace avitab {

struct JobInfo {
    int pageCount;
    int width;
    int height;
};

class RasterJob {
public:
    RasterJob(fz_context *fzCtx, const std::string &path);
    void setOutputBuf(uint32_t *buf, int width, int height);
    void rasterize(std::promise<JobInfo> result);

    ~RasterJob();

private:
    std::string docPath;
    fz_context *ctx = nullptr;

    // Document info, re-used
    fz_document *doc = nullptr;
    fz_page *page = nullptr;
    int curPage = -1;

    uint32_t *outBuf = nullptr;
    int outWidth = 0, outHeight = 0;

    void doWork(JobInfo &info);
    void openDocument(JobInfo &info);
    void loadPage(int pageNum);
    fz_matrix calculateScale();
    void rasterPage(JobInfo &info, fz_matrix &scaleMatrix);

};

} /* namespace avitab */

#endif /* SRC_GUI_TOOLKIT_RASTERIZERS_RASTERJOB_H_ */
