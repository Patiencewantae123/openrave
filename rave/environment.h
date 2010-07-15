// -*- coding: utf-8 -*-
// Copyright (C) 2006-2010 Rosen Diankov (rdiankov@cs.cmu.edu)
//
// This file is part of OpenRAVE.
// OpenRAVE is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
/**
\file   environment.h
\brief  Definition of the EnvironmentBase interface used for managing all objects in an environment.
 */
#ifndef  OPENRAVE_ENVIRONMENTBASE_H
#define  OPENRAVE_ENVIRONMENTBASE_H

namespace OpenRAVE {

typedef boost::recursive_try_mutex EnvironmentMutex;

/// Environment class
/// Holds everything necessary to load the environment, simulate a problem, and gather statistics.
/// It should be free of rendering and GUI work.
class EnvironmentBase : public boost::enable_shared_from_this<EnvironmentBase>
{
public:
    EnvironmentBase() {}
    virtual ~EnvironmentBase() {}

    /// releases all environment resources. Should be called whenever the environment stops being used
    /// (removing all environment pointer might not be enough to destroy the environment resources)
    virtual void Destroy()=0;

    /// Resets all objects of the scene (preserves all problems, planners)
    /// do not call inside a SimulationStep call
    virtual void Reset()=0;

    /// @name Interface Creation and Plugin Management
    //@{
    virtual InterfaceBasePtr CreateInterface(InterfaceType type,const std::string& interfacename)=0;
    virtual RobotBasePtr CreateRobot(const std::string& name="")=0;
    virtual PlannerBasePtr CreatePlanner(const std::string& name)=0;
    virtual SensorSystemBasePtr CreateSensorSystem(const std::string& name)=0;
    virtual ControllerBasePtr CreateController(const std::string& name)=0;
    virtual ProblemInstancePtr CreateProblem(const std::string& name)=0;
    virtual IkSolverBasePtr CreateIkSolver(const std::string& name)=0;
    virtual PhysicsEngineBasePtr CreatePhysicsEngine(const std::string& name)=0;
    virtual SensorBasePtr CreateSensor(const std::string& name)=0;
    virtual CollisionCheckerBasePtr CreateCollisionChecker(const std::string& name)=0;
    virtual RaveViewerBasePtr CreateViewer(const std::string& name)=0;
    
    /// \return an empty KinBody instance, deallocate with delete, physics needs to be locked
    virtual KinBodyPtr CreateKinBody(const std::string& name="") = 0;

    /// \return an empty trajectory instance initialized to nDOF degrees of freedom. Deallocate with delete.
    virtual TrajectoryBasePtr CreateTrajectory(int nDOF) = 0;

    /// environment will own the interface until Destroy is called
    virtual void OwnInterface(InterfaceBasePtr pinterface) = 0;

    /// environment owner if interface is removed
    virtual void DisownInterface(InterfaceBasePtr pinterface) = 0;

    /// returns true if interface can be loaded from a plugin, otherwise false
    virtual bool HasInterface(InterfaceType type, const std::string& interfacename) = 0;
    
    /// get all the loaded plugins and the interfaces they support
    /// \param plugins A list of plugins. Each entry has the plugin name and the interfaces it supports
    virtual void GetPluginInfo(std::list< std::pair<std::string, PLUGININFO> >& plugins)=0;

    /// get a list of all the loaded interfaces
    virtual void GetLoadedInterfaces(PLUGININFO& info) = 0;

    /// load a plugin and its interfaces
    /// \param name the filename of the plugin to load
    virtual bool LoadPlugin(const std::string& name) = 0;
    /// \brief Reloads all currently loaded plugins.
    ///
    /// The interfaces currently created remain will continue using the old plugins, so this function is safe in that plugins currently loaded remain loaded until the last interface that uses them is released.
    virtual void ReloadPlugins() = 0;
    //@}

    /// Returns a clone of the current environment. Clones do not share any memory or resource between each other
    /// or their parent making them ideal for performing separte planning experiments while keeping
    /// the parent environment unchanged.
    /// By default a clone only copies the collision checkers and physics engine.
    /// When bodies are cloned, the unique ids are preserved across environments (each body can be referenced with its id in both environments). The attached and grabbed bodies of each body/robot are also copied to the new environment.
    /// \param options A set of CloningOptions describing what is actually cloned.
    virtual EnvironmentBasePtr CloneSelf(int options) = 0;

    /// Each function takes an optional pointer to a CollisionReport structure and returns true if collision occurs.
    /// @name Collision specific functions.
    //@{
    /// set the global environment collision checker
    virtual bool SetCollisionChecker(CollisionCheckerBasePtr pchecker)=0;
    virtual CollisionCheckerBasePtr GetCollisionChecker() const =0;

    virtual bool CheckCollision(KinBodyConstPtr pbody1, CollisionReportPtr report = CollisionReportPtr())=0;
    virtual bool CheckCollision(KinBodyConstPtr pbody1, KinBodyConstPtr pbody2, CollisionReportPtr report = CollisionReportPtr())=0;
    virtual bool CheckCollision(KinBody::LinkConstPtr plink, CollisionReportPtr report = CollisionReportPtr())=0;
    virtual bool CheckCollision(KinBody::LinkConstPtr plink1, KinBody::LinkConstPtr plink2, CollisionReportPtr report = CollisionReportPtr())=0;
    virtual bool CheckCollision(KinBody::LinkConstPtr plink, KinBodyConstPtr pbody, CollisionReportPtr report = CollisionReportPtr())=0;
    
    virtual bool CheckCollision(KinBody::LinkConstPtr plink, const std::vector<KinBodyConstPtr>& vbodyexcluded, const std::vector<KinBody::LinkConstPtr>& vlinkexcluded, CollisionReportPtr report = CollisionReportPtr())=0;
    virtual bool CheckCollision(KinBodyConstPtr pbody, const std::vector<KinBodyConstPtr>& vbodyexcluded, const std::vector<KinBody::LinkConstPtr>& vlinkexcluded, CollisionReportPtr report = CollisionReportPtr())=0;

    /// Check collision with a link and a ray with a specified length.
    /// \param ray holds the origin and direction. The length of the ray is the length of the direction.
    /// \param plink the link to collide with
    /// \param[out] report [optional] collision report to be filled with data about the collision. If a body was hit, CollisionReport::plink1 contains the hit link pointer.
    virtual bool CheckCollision(const RAY& ray, KinBody::LinkConstPtr plink, CollisionReportPtr report = CollisionReportPtr()) = 0;

    /// Check collision with a link and a ray with a specified length.
    /// \param ray holds the origin and direction. The length of the ray is the length of the direction.
    /// \param pbody the link to collide with
    /// \param[out] report [optional] collision report to be filled with data about the collision. If a body was hit, CollisionReport::plink1 contains the hit link pointer.
    virtual bool CheckCollision(const RAY& ray, KinBodyConstPtr pbody, CollisionReportPtr report = CollisionReportPtr()) = 0;

    /// Check collision with a body and a ray with a specified length.
    /// \param ray holds the origin and direction. The length of the ray is the length of the direction.
    /// \param pbody the kinbody to look for collisions
    /// \param[out] report [optional] collision report to be filled with data about the collision. If a body was hit, CollisionReport::plink1 contains the hit link pointer.
    virtual bool CheckCollision(const RAY& ray, CollisionReportPtr report = CollisionReportPtr()) = 0;

    /// check self collision with the body
    virtual bool CheckSelfCollision(KinBodyConstPtr pbody, CollisionReportPtr report = CollisionReportPtr()) = 0;

    typedef boost::function<CollisionAction(CollisionReportPtr,bool)> CollisionCallbackFn;
        
    /// Register a collision callback.
    ///
    /// Whenever a collision is detected between between bodies during a CheckCollision call or physics simulation, the callback is called.
    /// The callback should return an action specifying how the collision should be handled:
    /// <b>action = callback(CollisionReport,bool IsCalledFromPhysicsEngine)</b>
    /// \return a handle to the registration, once the handle loses scope, the callback is unregistered
    virtual boost::shared_ptr<void> RegisterCollisionCallback(const CollisionCallbackFn& callback) = 0;
    virtual bool HasRegisteredCollisionCallbacks() const = 0;
    virtual void GetRegisteredCollisionCallbacks(std::list<CollisionCallbackFn>&) const = 0;
    //@}

    /// @name Physics and Simulation
    //@{
    /// set the physics engine, disabled by default
    /// \param the engine to set, if NULL, environment sets an dummy physics engine
    virtual bool SetPhysicsEngine(PhysicsEngineBasePtr pengine) = 0;
    virtual PhysicsEngineBasePtr GetPhysicsEngine() const = 0;

    /// Makes one simulation step
    virtual void StepSimulation(dReal timeStep) = 0;

    /// Start the internal physics engine loop, calls SimulateStep for all modules
    /// \param fDeltaTime the delta step to take in simulation
    /// \param bRealTime if false will call SimulateStep as fast as possible, otherwise will time the simulate step calls so that simulation progresses with real system time.
    virtual void StartSimulation(dReal fDeltaTime, bool bRealTime=true) = 0;

    /// Stops the internal physics loop, stops calling SimulateStep for all modules
    virtual void StopSimulation() = 0;

    /// \return true if inner simulation loop is executing
    virtual bool IsSimulationRunning() const = 0;
    
    /// \return simulation time since the start of the environment (in microseconds)
    virtual uint64_t GetSimulationTime() = 0;
    //@}

    /// @name XML Parsing, File Loading
    //@{ 
    /// Loads a scene from an XML file, environment is locked automatically making this method thread-safe
    virtual bool Load(const std::string& filename) = 0;
    /// Loads a scene from XML-formatted data, environment is locked automatically making this method thread-safe
    virtual bool LoadXMLData(const std::string& data) = 0;
    /// Saves a scene depending on the filename extension. Default is in COLLADA format
    virtual bool Save(const std::string& filename) = 0;
    /// Initializes a robot from an XML file. The robot should not be added the environment when calling this function.
    /// \param robot If a null pointer is passed, a new robot will be created, otherwise an existing robot will be filled
    /// \param filename the name of the file to open
    /// \param atts the XML attributes/value pairs
    virtual RobotBasePtr ReadRobotXMLFile(RobotBasePtr robot, const std::string& filename, const std::list<std::pair<std::string,std::string> >& atts) = 0;
    virtual RobotBasePtr ReadRobotXMLFile(const std::string& filename) = 0;

    /// Initialize a robot from an XML formatted string
    /// The robot should not be added the environment when calling this function.
    /// \param robot If a null pointer is passed, a new robot will be created, otherwise an existing robot will be filled
    /// \param atts the XML attributes/value pairs
    virtual RobotBasePtr ReadRobotXMLData(RobotBasePtr robot, const std::string& data, const std::list<std::pair<std::string,std::string> >& atts) = 0;

    /// Initializes a kinematic body from an XML file. The body should not be added to the environment when calling this function.
    /// \param filename the name of the file to open
    /// \param body If a null pointer is passed, a new body will be created, otherwise an existing robot will be filled
    /// \param atts the XML attributes/value pairs
    virtual KinBodyPtr ReadKinBodyXMLFile(KinBodyPtr body, const std::string& filename, const std::list<std::pair<std::string,std::string> >& atts) = 0;
    virtual KinBodyPtr ReadKinBodyXMLFile(const std::string& filename) = 0;

    /// Initializes a kinematic body from an XML formatted string.
    // The body should not be added to the environment when calling this function.
    /// \param body If a null pointer is passed, a new body will be created, otherwise an existing robot will be filled
    /// \param atts the XML attributes/value pairs
    virtual KinBodyPtr ReadKinBodyXMLData(KinBodyPtr body, const std::string& data, const std::list<std::pair<std::string,std::string> >& atts) = 0;

    /// Initializes an interface from an XML file.
    /// \param pinterface If a null pointer is passed, a new interface will be created, otherwise an existing interface will be filled
    /// \param filename the name of the file to open
    /// \param atts the XML attributes/value pairs
    virtual InterfaceBasePtr ReadInterfaceXMLFile(InterfaceBasePtr pinterface, InterfaceType type, const std::string& filename, const std::list<std::pair<std::string,std::string> >& atts) = 0;
    virtual InterfaceBasePtr ReadInterfaceXMLFile(const std::string& filename) = 0;

    /// Initializes an interface from an XML formatted string.
    /// \param pinterface If a null pointer is passed, a new interface will be created, otherwise an existing interface will be filled
    /// \param data string containing XML data
    /// \param atts the XML attributes/value pairs
    virtual InterfaceBasePtr ReadInterfaceXMLData(InterfaceBasePtr pinterface, InterfaceType type, const std::string& data, const std::list<std::pair<std::string,std::string> >& atts) = 0;

    typedef boost::function<BaseXMLReaderPtr(InterfaceBasePtr, const std::list<std::pair<std::string,std::string> >&)> CreateXMLReaderFn;

    /// registers a custom xml reader for a particular interface. Once registered, anytime an interface is created through XML and
    /// the xmltag is seen, the function CreateXMLReaderFn will be called to get a reader for that tag
    /// \param xmltag the tag specified in xmltag is seen in the interface, the the custom reader will be created.
    /// \param fn CreateXMLReaderFn(pinterface,atts) - passed in the pointer to the interface where the tag was seen along with the list of attributes
    /// \return a pointer holding the registration, releasing the pointer will unregister the XML reader
    virtual boost::shared_ptr<void> RegisterXMLReader(InterfaceType type, const std::string& xmltag, const CreateXMLReaderFn& fn) = 0;
    
    /// Parses a file for XML data
    virtual bool ParseXMLFile(BaseXMLReaderPtr preader, const std::string& filename) = 0;

    /// Parses a data file for XML data
    /// \param pdata The data of the buffer
    /// \param len the number of bytes valid in pdata
    virtual bool ParseXMLData(BaseXMLReaderPtr preader, const std::string& data) = 0;
    //@}

    /// @name Object Setting and Querying
    //@{
    
    /// add a body to the environment
    /// \param[in] body the pointer to an initialized body
    /// \param[in] bAnonymous if true and there exists a body with the same name, will make body's name unique
    virtual bool AddKinBody(KinBodyPtr body, bool bAnonymous=false) = 0;
    /// add a body to the environment
    /// \param[in] robot the pointer to an initialized body
    /// \param[in] bAnonymous if true and there exists a body with the same name, will make body's name unique
    virtual bool AddRobot(RobotBasePtr robot, bool bAnonymous=false) = 0;

    /// Removes a body from the environment [multi-thread safe].
    /// \param[in] body the body to remove
    virtual bool RemoveKinBody(KinBodyPtr body) = 0;

    /// Query a body from its name [multi-thread safe].
    /// \return first KinBody (including robots) that matches with name
    virtual KinBodyPtr GetKinBody(const std::string& name)=0;

    /// Query a robot from its name [multi-thread safe].
    /// \return first Robot that matches the name
    virtual RobotBasePtr GetRobot(const std::string& name)=0;

    /// Get all bodies loaded in the environment (including robots) [multi-thread safe]
    /// \paramp[out] bodies filled with all the bodies
    virtual void GetBodies(std::vector<KinBodyPtr>& bodies) const = 0;

    /// fill an array with all robots loaded in the environment
    virtual void GetRobots(std::vector<RobotBasePtr>& robots) const = 0;
    
    /// retrieve published bodies, note that the pbody pointer might become invalid
    /// as soon as GetPublishedBodies returns
    virtual void GetPublishedBodies(std::vector<KinBody::BodyState>& vbodies) = 0;

    /// updates the published bodies that viewers and other programs listening in on the environment see.
    /// For example, calling this function inside a planning loop allows the viewer to update the environment
    /// reflecting the status of the planner.
    /// Assumes that the physics are locked. 
    virtual void UpdatePublishedBodies() = 0;

    /// Get the corresponding body from its unique network id
    virtual KinBodyPtr GetBodyFromEnvironmentId(int id) = 0;
    virtual KinBodyPtr GetBodyFromNetworkId(int id) RAVE_DEPRECATED = 0;

    /// A set of options specifying what to triangulate
    enum TriangulateOptions
    {
        TO_Obstacles = 1,   ///< everything but robots
        TO_Robots = 2,      ///< all robots
        TO_Everything = 3,  ///< all bodies and robots everything
        TO_Body = 4,        ///< only triangulate kinbody
        TO_AllExceptBody = 5 ///< triangulate everything but kinbody
    };
    
    /// triangulation of the body including its current transformation. trimesh will be appended the new data.
    virtual bool Triangulate(KinBody::Link::TRIMESH& trimesh, KinBodyConstPtr pbody) = 0;

    /// general triangulation of the whole scene. trimesh will be appended the new data.
    /// \param opts - Controlls what to triangulate
    virtual bool TriangulateScene(KinBody::Link::TRIMESH& trimesh, TriangulateOptions opts, const std::string& name) = 0;
    //@}

    /// Load a new problem, need to Lock if calling outside simulation thread
    virtual int LoadProblem(ProblemInstancePtr prob, const std::string& cmdargs) = 0;
    /// Load a new problem, need to Lock if calling outside simulation thread
    virtual bool RemoveProblem(ProblemInstancePtr prob) = 0;

    /// Returns a list of loaded problems with a lock. As long as the lock is held, the problems
    /// are guaranteed to stay loaded in the environment.
    /// \return returns a pointer to a Lock. Destroying the shared_ptr will release the lock
    virtual boost::shared_ptr<void> GetLoadedProblems(std::list<ProblemInstancePtr>& listProblems) const = 0;

    /// Lock/unlock the environment mutex. Accessing environment body information and adding/removing bodies
    /// or changing any type of scene property should have the environment lock acquired. Once the environment
    /// is locked, the user is guaranteed that nnothing will change in the environment.
    /// \return the mutex used to control the lock.
    virtual EnvironmentMutex& GetMutex() const = 0;

    virtual bool AttachViewer(RaveViewerBasePtr pnewviewer) = 0;
    virtual RaveViewerBasePtr GetViewer() const = 0;

    /// @name 3D plotting methods. All plotting calls are thread safe
    //@{

    typedef boost::shared_ptr<void> GraphHandlePtr;

    /// \param ppoints array of points
    /// \param numPoints number of points to plot
    /// \param stride stride in bytes to next point, ie: nextpoint = (float*)((char*)ppoints+stride)
    /// \param fPointSize size of a point in pixels
    /// \param color the rgb color of the point. The last component of the color is used for alpha blending
    /// \param drawstyle if 0 will draw pixels. if 1, will draw 3D spheres
    /// \return handle to plotted points, graph is removed when handle is destroyed (goes out of scope). This requires the user to always store the handle in a persistent variable if the plotted graphics are to remain on the viewer.
    virtual GraphHandlePtr plot3(const float* ppoints, int numPoints, int stride, float fPointSize, const RaveVector<float>& color = RaveVector<float>(1,0.5,0.5,1), int drawstyle = 0) = 0;

    /// plots 3D points. Arguments same as plot3 with one color, except has an individual color for every point
    /// \param colors An array of rgb colors of size numPoints where each channel is in [0,1].
    ///               colors+(bhasalpha?4:3) points to the second color.
    /// \param stride stride in bytes to next point, ie: nextpoint = (float*)((char*)ppoints+stride)
    /// \param drawstyle if 0 will draw pixels. if 1, will draw 3D spherse
    /// \param bhasalpha if true, then each color consists of 4 values with the last value being the alpha of the point (1 means opaque). If false, then colors is 3 values.
    /// \return handle to plotted points, graph is removed when handle is destroyed (goes out of scope). This requires the user to always store the handle in a persistent variable if the plotted graphics are to remain on the viewer.
    virtual GraphHandlePtr plot3(const float* ppoints, int numPoints, int stride, float fPointSize, const float* colors, int drawstyle = 0, bool bhasalpha = false) = 0;
    
    /// draws a series of connected lines
    /// \param stride stride in bytes to next point, ie: nextpoint = (float*)((char*)ppoints+stride)
    /// \param color the rgb color of the point. The last component of the color is used for alpha blending
    /// \return handle to plotted points, graph is removed when handle is destroyed (goes out of scope). This requires the user to always store the handle in a persistent variable if the plotted graphics are to remain on the viewer.
    virtual GraphHandlePtr drawlinestrip(const float* ppoints, int numPoints, int stride, float fwidth, const RaveVector<float>& color = RaveVector<float>(1,0.5,0.5,1)) = 0;

    /// \param stride stride in bytes to next point, ie: nextpoint = (float*)((char*)ppoints+stride)
    /// \return handle to plotted points, graph is removed when handle is destroyed (goes out of scope). This requires the user to always store the handle in a persistent variable if the plotted graphics are to remain on the viewer.
    virtual GraphHandlePtr drawlinestrip(const float* ppoints, int numPoints, int stride, float fwidth, const float* colors) = 0;

    /// draws a list of individual lines, each specified by a succeeding pair of points
    /// \param stride stride in bytes to next point, ie: nextpoint = (float*)((char*)ppoints+stride)
    /// \param color the rgb color of the point. The last component of the color is used for alpha blending.
    /// \return handle to plotted points, graph is removed when handle is destroyed (goes out of scope). This requires the user to always store the handle in a persistent variable if the plotted graphics are to remain on the viewer.
    virtual GraphHandlePtr drawlinelist(const float* ppoints, int numPoints, int stride, float fwidth, const RaveVector<float>& color = RaveVector<float>(1,0.5,0.5,1)) = 0;

    /// \param stride stride in bytes to next point, ie: nextpoint = (float*)((char*)ppoints+stride)
    /// \return handle to plotted points, graph is removed when handle is destroyed (goes out of scope). This requires the user to always store the handle in a persistent variable if the plotted graphics are to remain on the viewer.
    virtual GraphHandlePtr drawlinelist(const float* ppoints, int numPoints, int stride, float fwidth, const float* colors) = 0;

    /// draws an arrow p1 is start, p2 is finish
    /// \param color the rgb color of the point. The last component of the color is used for alpha blending.
    /// \return handle to plotted points, graph is removed when handle is destroyed (goes out of scope). This requires the user to always store the handle in a persistent variable if the plotted graphics are to remain on the viewer.
    virtual GraphHandlePtr drawarrow(const RaveVector<float>& p1, const RaveVector<float>& p2, float fwidth, const RaveVector<float>& color = RaveVector<float>(1,0.5,0.5,1)) = 0;
    
    /// draws a box
    /// extents are half the width, height, and depth of the box
    /// \return handle to plotted points, graph is removed when handle is destroyed (goes out of scope). This requires the user to always store the handle in a persistent variable if the plotted graphics are to remain on the viewer.
    virtual GraphHandlePtr drawbox(const RaveVector<float>& vpos, const RaveVector<float>& vextents) = 0;

    /// draws a textured plane
    /// \param tplane describes the center of the plane. the zaxis of this coordinate is the normal of the plane
    /// \param vextents the extents of the plane along the x and y directions (z is ignored)
    /// \param vtexture a 3D array specifying height x width x color (the color dimension can be 1, 3, or 4 (for alpha blending))
    /// \return handle to plotted points, graph is removed when handle is destroyed (goes out of scope). This requires the user to always store the handle in a persistent variable if the plotted graphics are to remain on the viewer.
    virtual GraphHandlePtr drawplane(const RaveTransform<float>& tplane, const RaveVector<float>& vextents, const boost::multi_array<float,3>& vtexture) = 0;

    /// draws a triangle mesh, each vertices of each triangle should be counter-clockwise.
    /// \param ppoints - array of 3D points
    /// \param stride stride in bytes to next point, ie: nextpoint = (float*)((char*)ppoints+stride)
    /// \param pIndices If not NULL, zero-based indices into the points for every triangle. pIndices
    /// should be of size numTriangles. If pIndices is NULL, ppoints is assumed to contain numTriangles*3
    /// points and triangles will be rendered in list order.
    /// \param color The color of the triangle. The last component of the color is used for alpha blending
    /// \return handle to plotted points, graph is removed when handle is destroyed (goes out of scope). This requires the user to always store the handle in a persistent variable if the plotted graphics are to remain on the viewer.
    virtual GraphHandlePtr drawtrimesh(const float* ppoints, int stride, const int* pIndices, int numTriangles, const RaveVector<float>& color) = 0;
    virtual GraphHandlePtr drawtrimesh(const float* ppoints, int stride, const int* pIndices, int numTriangles, const boost::multi_array<float,2>& colors) = 0;
    //@}

    /// returns the openrave home directory where settings, cache, and other files are stored.
    /// On Linux/Unix systems, this is usually $HOME/.openrave, on Windows this is $HOMEPATH/.openrave
    virtual const std::string& GetHomeDirectory() const = 0;

    //@{ debug/global commands
    
    /// sets the debug level
    /// \param level 0 for no debug, 1 - to print all debug messeges. Default
    ///             value for release builds is 0, for debug builds it is 1
    /// declaring variables with stdcall can be a little complex
    virtual void SetDebugLevel(DebugLevel level) = 0;
    virtual DebugLevel GetDebugLevel() const = 0;
    //@}

protected:
    virtual const char* GetHash() const { return OPENRAVE_ENVIRONMENT_HASH; }
};

} // end namespace OpenRAVE

#endif