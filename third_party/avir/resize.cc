/*-*-mode:c++;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8-*-│
│vi: set net ft=c++ ts=2 sts=2 sw=2 fenc=utf-8                              :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/avir/resize.h"
namespace {
#include "third_party/avir/avir_float4_sse.h"
}  // namespace

struct ResizerImpl {
  ResizerImpl() : resizer{8, 8, avir::CImageResizerParamsULR()} {}
  avir::CImageResizer<avir::fpclass_float4> resizer;
};

void NewResizer(Resizer *resizer, int aResBitDepth, int aSrcBitDepth) {
  FreeResizer(resizer);
  resizer->p = new ResizerImpl();
}

void FreeResizer(Resizer *resizer) {
  if (!resizer->p) return;
  delete (ResizerImpl *)resizer->p;
  resizer->p = nullptr;
}

void ResizeImage(Resizer *resizer, float *Dest, int DestHeight, int DestWidth,
                 const float *Src, int SrcHeight, int SrcWidth) {
  ResizerImpl *impl = (ResizerImpl *)resizer->p;
  int SrcScanLineSize = 0;
  double ResizingStep = 0;
  impl->resizer.resizeImage(Src, SrcWidth, SrcHeight, SrcScanLineSize, Dest,
                            DestWidth, DestHeight, 4, ResizingStep);
}
