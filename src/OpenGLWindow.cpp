/*
 * Basic GL Window modified from the example here
 * http://qt-project.org/doc/qt-5.0/qtgui/openglwindow.html
 * adapted to use NGL
 */
#include "OpenGLWindow.h"
#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>
#include <iostream>
#include "Context.h"

OpenGLWindow::OpenGLWindow(QWindow *_parent)
    : QWindow(_parent)
    , m_updatePending(false)
    , m_contextCreated(false)
{
  // ensure we render to OpenGL and not a QPainter by setting the surface type
  setSurfaceType(QWindow::OpenGLSurface);
}

OpenGLWindow::~OpenGLWindow()
{
  // now we have finished clear the device
 // delete m_device;
}


void OpenGLWindow::renderLater()
{
  // this method allows us to post an event and render once the system is ready see the event processing
  // code below
  if (!m_updatePending)
  {
    m_updatePending = true;
    // signal and update request
    QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
  }
}

bool OpenGLWindow::event(QEvent *event)
{
  switch (event->type())
  {
  case QEvent::UpdateRequest:
    renderNow();
    return true;

  default:
    return QWindow::event(event);
  }
}

void OpenGLWindow::exposeEvent(QExposeEvent *event)
{
  // don't use the event
  Q_UNUSED(event);
  // if the window is exposed (visible) render
  if (isExposed())
  {
    renderNow();
  }
}


void OpenGLWindow::renderNow()
{
  // no need to draw if window is hidden
  if (!isExposed())
  {
    return;
  }

  m_updatePending = false;

  // if this is the first time round init the GL context this will only be called once
  if (!m_contextCreated)
  {
    m_contextCreated=true;
    Context *context=Context::instance(this);
    context->makeCurrent(this);
    // now call the int method in our child class to do all the one time GL init stuff
    initialize();

  }
  Context *context=Context::instance(this);

  // usually we will make this context current and render
  context->makeCurrent(this);
  // call the render in the child class (NGLScene)
  render();
  // finally swap the buffers to make visible
  context->swapBuffers(this);
}


