#ifndef TESTS_UTILS_PORTFORWARDER
#define TESTS_UTILS_PORTFORWARDER

#include <thread>
#include <vector>

using namespace std;

/**
 * @brief A test utility class for forwarding data through a socket.
 * Allows the control of data to simulate lost data or disconnections
 */
class PortForwarder
{
private:
    int inPort, outPort;
    int inSocket, outSocket;
    bool isStopped = false;

    vector<thread *> threads;

    /**
     * @brief Opens a listener socket
     *
     * @param port
     * @return int
     */
    static int openListener(int port);

    /**
     * @brief Opens an output socket
     *
     * @param port
     * @return int
     */
    static int openOutput(int port);

    /**
     * @brief Main function that will be run on a seperate thread
     * Will monitor for new connections and setup a pair of connections for forwarding the data
     */
    void main();

public:
    PortForwarder(PortForwarder &&) = default;
    PortForwarder(int inPort, int outPort) : inPort(inPort), outPort(outPort){};
    ~PortForwarder();

    /**
     * @brief Starts port forwarding
     *
     * @return int returns non 0 if it failed to setup forwarding
     */
    int start();

    /**
     * @brief Stops port forwarding, and closes active threads.
     *
     */
    void stop();

    /**
     * @brief Utility method for checking if a port is open
     * Blocking call and will retry until the port is open
     *
     * @param port
     * @param maxRetries
     * @return true
     * @return false
     */
    static bool portCheck(int port, int maxRetries = 5);
};

#endif /* TESTS_UTILS_PORTFORWARDER */
