////
///
///     sets up the main navigation structure of the app.
///
///     Takes a userRole parameter to customize the navigation based on the user's role.
///
///

////

import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'screens/home_page.dart';
import 'screens/live_camera_page.dart';
import 'screens/settings_page.dart';

class AppNavigator extends StatefulWidget {
  final String userRole; // Add userRole parameter

  const AppNavigator({super.key, required this.userRole}); // Constructor with userRole

  @override
  State<AppNavigator> createState() => _AppNavigatorState();
}


//Manages the state and behavior of the AppNavigator widget.
class _AppNavigatorState extends State<AppNavigator> {
  int _currentIndex = 0;

  // Titles for the AppBar corresponding to each tab
  final _titles = [
    "Home",
    "Live Camera",
    "Settings",
  ];

  // Function to handle logout
  void _logout() async {
    await FirebaseAuth.instance.signOut(); // Sign out the user
    Navigator.pushReplacementNamed(context, '/login'); // Navigate to LoginScreen
  }

  @override
  Widget build(BuildContext context) {
    // Tabs with userRole passed where needed
    final _tabs = [
      Home(userRole: widget.userRole),      // Pass userRole to HomePage
      LiveCameraScreen(),                       // No role-specific logic for Live Camera
      SettingsScreen(userRole: widget.userRole) // Pass userRole to SettingsScreen
    ];

    return Scaffold(
      appBar: AppBar(
        title: Text(_titles[_currentIndex]), // Dynamic title based on selected tab
        backgroundColor: Theme.of(context).colorScheme.inversePrimary,
        actions: [
          IconButton(
            icon: const Icon(Icons.logout),
            tooltip: 'Logout',
            onPressed: _logout, // Call logout logic
          ),
        ],
      ),
      /// The IndexedStack widget in the body of the Scaffold is used to manage the display of multiple
      /// child widgets, showing only one child at a time based on the current index. It helps in maintaining
      /// the state of each child widget even when it is not visible.
      body: IndexedStack(
        index: _currentIndex,
        children: _tabs,
      ),
      bottomNavigationBar: NavigationBar(
        selectedIndex: _currentIndex,
        onDestinationSelected: (index) {
          setState(() {
            _currentIndex = index;
          });
        },
        destinations: const [
          NavigationDestination(icon: Icon(Icons.home), label: 'Home'),
          NavigationDestination(icon: Icon(Icons.videocam), label: 'Live Camera'),
          NavigationDestination(icon: Icon(Icons.settings), label: 'Settings'),
        ],
      ),
    );
  }
}
