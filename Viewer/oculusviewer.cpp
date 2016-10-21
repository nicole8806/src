/*
 * oculusviewer.cpp
 *
 *  Created on: Jun 30, 2013
 *      Author: Jan Ciger & Bj�rn Blissing
 */

#include "oculusviewer.h"
#include "oculusupdateslavecallback.h"

#include <iostream>

#include <osg/Camera>

#include "Renderer.hxx"

#include <osgViewer/Renderer>
#include <simgear/scene/material/EffectCullVisitor.hxx>

//#include "Drawable.hxx"
#include "Viewer.hxx"
//#include "SlaveCamera.hxx"

#include <simgear/scene/util/RenderConstants.hxx>

extern osg::Matrix cameraMx;
extern osg::Camera* cameraEx;
extern osg::Node::NodeMask cullMaskl;

osg::ref_ptr<osg::Camera> cameraRTTLeft;
osg::ref_ptr<osg::Camera> cameraRTTRight;


using namespace osg;
using namespace simgear;
using namespace flightgear;
//XUY---------------------------------------
//add texture and light
void installCullVisitor(osg::observer_ptr<osg::Camera> camera)
{
    osgViewer::Renderer* renderer
        = static_cast<osgViewer::Renderer*>(camera->getRenderer());
    for (int i = 0; i < 2; ++i) {
        osgUtil::SceneView* sceneView = renderer->getSceneView(i);

        osg::ref_ptr<osgUtil::CullVisitor::Identifier> identifier;
        identifier = sceneView->getCullVisitor()->getIdentifier();
        sceneView->setCullVisitor(new simgear::EffectCullVisitor);
        sceneView->getCullVisitor()->setIdentifier(identifier.get());

        //identifier = sceneView->getCullVisitorLeft()->getIdentifier();
        //sceneView->setCullVisitorLeft(sceneView->getCullVisitor()->clone());
        //sceneView->getCullVisitorLeft()->setIdentifier(identifier.get());

        //identifier = sceneView->getCullVisitorRight()->getIdentifier();
        //sceneView->setCullVisitorRight(sceneView->getCullVisitor()->clone());
        //sceneView->getCullVisitorRight()->setIdentifier(identifier.get());

    }
}
//xuy---------------------------------------------

void OCsetCameraCullMasks(osg::Node::NodeMask nm)
{

  //      cameraRTTLeft->setCullMask(nm & ~simgear::BACKGROUND_BIT);
		//cameraRTTLeft->setCullMaskLeft(nm & ~simgear::BACKGROUND_BIT);
  //      cameraRTTLeft->setCullMaskRight(nm & ~simgear::BACKGROUND_BIT);
        cameraRTTLeft->setCullMask(nm);
        cameraRTTLeft->setCullMaskLeft(nm);
        cameraRTTLeft->setCullMaskRight(nm);
		cameraRTTRight->setCullMask(nm);
        cameraRTTRight->setCullMaskLeft(nm);
        cameraRTTRight->setCullMaskRight(nm);
          
}


/* Public functions */
void OculusViewer::traverse(osg::NodeVisitor& nv)
{
	// Must be realized before any traversal
	if (m_realizeOperation->realized())
	{
		if (!m_configured)
		{
			configure();
		}
	}

	osg::Group::traverse(nv);
}

/* Protected functions */
void OculusViewer::configure()
{
	osg::ref_ptr<osg::GraphicsContext> gc =  m_view->getCamera()->getGraphicsContext();

	// Attach a callback to detect swap
	osg::ref_ptr<OculusSwapCallback> swapCallback = new OculusSwapCallback(m_device);
	gc->setSwapCallback(swapCallback);

	osg::ref_ptr<osg::Camera> camera = m_view->getCamera();
	camera->setName("Main");
	osg::Vec4 clearColor = camera->getClearColor();

	// master projection matrix
	camera->setProjectionMatrix(m_device->projectionMatrixCenter());
	// Create RTT cameras and attach textures
	m_cameraRTTLeft = m_device->createRTTCamera(OculusDevice::LEFT, osg::Camera::RELATIVE_RF, clearColor, gc);
	m_cameraRTTRight = m_device->createRTTCamera(OculusDevice::RIGHT, osg::Camera::RELATIVE_RF, clearColor, gc);
	m_cameraRTTLeft->setName("LeftRTT");
	m_cameraRTTRight->setName("RightRTT");

	// Add RTT cameras as slaves, specifying offsets for the projection
	m_view->addSlave(m_cameraRTTLeft.get(),
					 m_device->projectionOffsetMatrixLeft(),
					 m_device->viewMatrixLeft(),
					 true);
	installCullVisitor(m_cameraRTTLeft);
	m_view->getSlave(0)._updateSlaveCallback = new OculusUpdateSlaveCallback(OculusUpdateSlaveCallback::LEFT_CAMERA, m_device.get(), swapCallback.get(),m_cameraRTTLeft);
	
	m_view->addSlave(m_cameraRTTRight.get(),
					 m_device->projectionOffsetMatrixRight(),
					 m_device->viewMatrixRight(),
					 true);
	installCullVisitor(m_cameraRTTRight);
	m_view->getSlave(1)._updateSlaveCallback = new OculusUpdateSlaveCallback(OculusUpdateSlaveCallback::RIGHT_CAMERA, m_device.get(), swapCallback.get(),m_cameraRTTRight);

	// Use sky light instead of headlight to avoid light changes when head movements
	m_view->setLightingMode(osg::View::SKY_LIGHT);

	// Disable rendering of main camera since its being overwritten by the swap texture anyway
	camera->setGraphicsContext(nullptr);

	m_configured = true;
}
