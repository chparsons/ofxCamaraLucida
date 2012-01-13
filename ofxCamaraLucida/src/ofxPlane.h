//	Copyright (C) 2011  Christian Parsons
//	www.chparsons.com.ar
//
//	based on:
//
//	@class plane
//	@ingroup NebulaMathDataTypes
//
//	A plane in 3d space.
//
//	(C) 2004 RadonLabsGmbH
//	http://www.koders.com/cpp/fid073EDD7A6ABCDD61FF7C128FE3563F9C75AD61ED.aspx
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "ofUtils.h"
#include "ofVectorMath.h"

class ofxPlane 
{
public:
	ofxPlane();
	ofxPlane(float A, float B, float C, float D);
	ofxPlane(const ofxPlane& p);
	ofxPlane(const ofVec3f& v0, const ofVec3f& v1, const ofVec3f& v2);
	
	void set(float A, float B, float C, float D);
	
	void set(const ofVec3f& v0, const ofVec3f& v1, const ofVec3f& v2);
	float distance(const ofVec3f& v) const;
	ofVec3f normal() const;
//    bool intersect(const line3& l, float& t) const;
//    bool intersect(const ofxPlane& p, line3& l) const;
	
	string toString();

	float a, b, c, d;
};

inline ofxPlane::ofxPlane() :
	a(0.0f),
	b(0.0f),
	c(0.0f),
	d(1.0f)
{
	// empty
}

inline ofxPlane::ofxPlane(float A, float B, float C, float D) :
	a(A),
	b(B),
	c(C),
	d(D)
{
	// empty
}

inline ofxPlane::ofxPlane(const ofxPlane& rhs) :
	a(rhs.a),
	b(rhs.b),
	c(rhs.c),
	d(rhs.d)
{
	// empty
}

inline string ofxPlane::toString()
{
	return "[ofxPlane a: "+ofToString(this->a)+" b: "+ofToString(this->b)+" c: "+ofToString(this->c)+" d: "+ofToString(this->d)+" ]";
}

inline void ofxPlane::set(float A, float B, float C, float D)
{
	this->a = A;
	this->b = B;
	this->c = C;
	this->d = D;
}

/**
	Constructs a Plane from 3 position vectors.
*/
inline void ofxPlane::set(const ofVec3f& v0, const ofVec3f& v1, const ofVec3f& v2)
{
	ofVec3f _normal = (v2 - v0).cross(v1 - v0);
	_normal.normalize();
	this->a = _normal.x;
	this->b = _normal.y;
	this->c = _normal.z;
	this->d = -(a * v0.x + b * v0.y + c * v0.z);
}

inline ofxPlane::ofxPlane(const ofVec3f& v0, const ofVec3f& v1, const ofVec3f& v2)
{
	this->set(v0, v1, v2);
}

/**
	Computes the distance of a point to the Plane. Return 0.0 if the 
	point is on the ofxPlane.
*/
inline float ofxPlane::distance(const ofVec3f& v) const
{
	return this->a * v.x + this->b * v.y + this->c * v.z + this->d;
}

/**
	Returns the Plane normal.
*/
inline ofVec3f ofxPlane::normal() const
{
	return ofVec3f(this->a, this->b, this->c);
}

/**
	Get intersecting t of line with one-sided Plane. Returns false
	if the line is parallel to the Plane.

	If the given line intersects with the Plane, plugging the value <i>t</i>
	into the line equation produces the point of intersection.

	The following code snippet shows the way to get the intersection point 
	between a line and a Plane:
	@code
	if (p.intersect(l, t))
	{
		ofVec3f intersection = l.b + t*l.m;
	}
	@endcode
*/
//inline bool ofxPlane::intersect(const line3& l, float& t) const
//{
//    float f0 = this->a * l.b.x + this->b * l.b.y + this->c * l.b.z + this->d;
//    float f1 = this->a * -l.m.x + this->b * -l.m.y + this->c * -l.m.z;
//    if ((f1 < -0.0001f) || (f1 > 0.0001f))
//    {
//        t = f0 / f1;
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}

/**
	Get Plane/Plane intersection. Return false if Planes are parallel.
*/
//inline bool ofxPlane::intersect(const ofxPlane& p, line3& l) const
//{
//    ofVec3f n0 = this->normal();
//    ofVec3f n1 = p.normal();
//    float n00 = n0 % n0;
//    float n01 = n0 % n1;
//    float n11 = n1 % n1;
//    float det = n00 * n11 - n01 * n01;
//    const float tol = 1e-06f;
//    if (fabs(det) < tol) 
//    {
//        return false;
//    }
//    else 
//    {
//        float inv_det = 1.0f/det;
//        float c0 = (n11 * this->d - n01 * p.d) * inv_det;
//        float c1 = (n00 * p.d - n01 * this->d) * inv_det;
//        l.m = n0 * n1;
//        l.b = n0 * c0 + n1 * c1;
//        return true;
//    }
//}
