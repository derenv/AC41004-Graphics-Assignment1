/**
* WindmillBase.h
* Example of a windmillbase. This was created by David Ogle in 2015 and updated and tidied up
* by Iain Martin in November 2017.
* Provided to the AC41001/AC51008 Graphics class to help debug their own windmillbase objects or to
* used in their assignment to provide another object to create models.
*
* The major limitation is that it is hard-coded to use 100 vertices in each rim. I would advise generalising
* this in in functions defineVertices() and makeWindmillBase() so this will only work if it is defined with a 
* definition of 100.
*/

#include "windmillbase.h"
const float PI = 3.141592653589f;  /* pi */

#include <iostream>

using namespace glm;
using namespace std;


/**
 * IM: Constrcutor with no parameters which creates a white windmillbase
 */
WindmillBase::WindmillBase () : WindmillBase(vec3(1.f, 1.f, 1.f))
{
	//
}


WindmillBase::WindmillBase(vec3 c) : colour(c)
{
	//
}

WindmillBase::~WindmillBase(){}

void WindmillBase::makeWindmillBase()
{
	//
}

void WindmillBase::drawWindmillBase(int drawmode)
{
	//
}