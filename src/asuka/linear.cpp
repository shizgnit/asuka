/*
================================================================================
  $RCSfile: uuid.cpp,v $ $Revision: 1.2 $ $State: Exp $
--------------------------------------------------------------------------------

  Copyright (c) 2005, Dee E. Abbott
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

================================================================================
*/


#include "linear.hpp"

__ASUKA_BEGIN_NAMESPACE

AT::Float32 Vector::dot(AT::Field &v1) {
  return(v1[0] * v1[0] +
         v1[1] * v1[1] +
         v1[2] * v1[2]);
}

AT::Float32 Vector::length(AT::Field &v1) {
  return(sqrt(dot(v1)));
}

AT::Field Vector::unit(AT::Field &v1) {
  AT::Field result;
  AT::Float32 length = Vector::length(v1);
  result[0] = v1[0] / length;
  result[1] = v1[1] / length;
  result[2] = v1[2] / length;
  return(result);
}

AT::Field Vector::cross(AT::Field &v1, AT::Field &v2) {
  AT::Field result;
  result[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
  result[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
  result[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
  return(result);
}

AT::Field Vector::diff(AT::Field &v1, AT::Field &v2) {
  AT::Field result;
  result[0] = v1[0] - v2[0];
  result[1] = v1[1] - v2[1];
  result[2] = v1[2] - v2[2];
  return(result);
}

AT::Field Surface::normal(AT::Field &s1) {
  AT::Field result;
  if(s1.size() >= 3) {
    AT::Field left = AT::Vector::diff(s1[1][0], s1[0][0]);
    AT::Field right = AT::Vector::diff(s1[2][0], s1[0][0]);
    result = AT::Vector::cross(left, right);
    result = AT::Vector::unit(result);
  }
  return(result);
}


__ASUKA_END_NAMESPACE

// Local Variables:
// mode:C++
// End:
