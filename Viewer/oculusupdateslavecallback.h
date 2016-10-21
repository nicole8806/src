/*
 * oculusupdateslavecallback.h
 *
 *  Created on: Jul 07, 2015
 *      Author: Björn Blissing
 */

#ifndef _OSG_OCULUSUPDATESLAVECALLBACK_H_
#define _OSG_OCULUSUPDATESLAVECALLBACK_H_

#include <osgViewer/View>

#include "oculusdevice.h"


struct OculusUpdateSlaveCallback : public osg::View::Slave::UpdateSlaveCallback
{
	enum CameraType
	{
		LEFT_CAMERA,
		RIGHT_CAMERA
	};

	OculusUpdateSlaveCallback(CameraType cameraType, OculusDevice* device, OculusSwapCallback* swapCallback,osg::ref_ptr<osg::Camera> camera) :
		m_cameraType(cameraType),
		m_device(device),
		//===========
		m_camera(camera),
		//=========
		m_swapCallback(swapCallback) {}

	virtual void updateSlave(osg::View& view, osg::View::Slave& slave);

	CameraType m_cameraType;
	osg::ref_ptr<OculusDevice> m_device;
	//============================nicole=======================
	osg::ref_ptr<osg::Camera> m_camera;
	//============================nicole========================
	osg::ref_ptr<OculusSwapCallback> m_swapCallback;
};

#endif // _OSG_OCULUSUPDATESLAVECALLBACK_H_
