///////////////////////////////////////////////////////////////////////////
///
///     Initializes Firebase
///
///     Uses AuthWrapper to check the user’s authentication state.
///
///     Sets up the main application widget.
///
///     Determines whether to show the LoginScreen or AppNavigator.
///
///     Handles FCM for foreground and background notifications.
///
///     Listens for Realtime Database changes and triggers notifications.
///
////////////////////////////////////////////////////////////////////////////

import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:firebase_messaging/firebase_messaging.dart'; // Import for FCM
import 'package:flutter_local_notifications/flutter_local_notifications.dart'; // Import for local notifications
import 'screens/login_screen.dart';
import 'app_navigator.dart';
import 'screens/settings/manage_residents_page.dart';
import 'screens/settings/password_management_page.dart';
import 'screens/settings/app_preferences_page.dart';

// Global instance of FlutterLocalNotificationsPlugin for local notifications
final FlutterLocalNotificationsPlugin flutterLocalNotificationsPlugin = FlutterLocalNotificationsPlugin();


///////////////////////////////////////////////////////////////////////////
Future<String> _fetchUserRole(String uid) async {
    try {
      // Fetch the role from the database
      DatabaseReference roleRef = FirebaseDatabase.instance.ref('users/$uid/role');

      // This method fetches the data at the specified database reference (roleRef) once.
      DataSnapshot snapshot = await roleRef.get();

      if (snapshot.exists) {
        // The user already has a role, return it as a string
        return snapshot.value as String;
      } else {
        // Default role to 'resident' if no role is defined
        return 'resident';
      }
    } catch (e) {
      // Default to 'resident' in case of an error
      return 'resident';
    }
  }
  
/// Function to initialize local notifications
Future<void> _initializeLocalNotifications() async {
  const AndroidInitializationSettings initializationSettingsAndroid =
      AndroidInitializationSettings('@mipmap/ic_launcher');

  const InitializationSettings initializationSettings =
      InitializationSettings(android: initializationSettingsAndroid);

  await flutterLocalNotificationsPlugin.initialize(initializationSettings);
}

/// Function to handle FCM messages in the foreground
/// When the app is in the foreground, Firebase does not automatically show notifications.
/// Therefore, we trigger local notifications manually.
void _handleForegroundMessage(RemoteMessage message) {
  RemoteNotification? notification = message.notification;
  AndroidNotification? android = message.notification?.android;

  if (notification != null && android != null) {
    flutterLocalNotificationsPlugin.show(
      notification.hashCode,
      notification.title,
      notification.body,
      const NotificationDetails(
        android: AndroidNotificationDetails(
          'high_importance_channel', // Channel ID
          'High Importance Notifications', // Channel name
          importance: Importance.max,
        ),
      ),
    );
  }
}


/// Function to retrieve and print the FCM token for testing notifications
/// The FCM token is used to identify the device for push notifications.
void printFCMToken() async {
  try {
    String? token = await FirebaseMessaging.instance.getToken();
    print("FCM Token: $token");
  } catch (e) {
    print("Failed to retrieve FCM token: $e");
  }
}

///////////////////////////////////////////////////////////////////////////
void main() async {
  /// Ensures that the Flutter framework is fully set up before any other code runs.
  /// This is particularly important when you need to perform
  /// asynchronous operations, such as initializing Firebase, before the app starts.
  WidgetsFlutterBinding.ensureInitialized();

  // Initialize Firebase
  await Firebase.initializeApp();

  // Initialize local notifications for FCM
  await _initializeLocalNotifications();

  // Print the FCM token for testing notifications
  printFCMToken(); // Call the function to print FCM token

  // Listen for FCM messages when the app is in the foreground
  FirebaseMessaging.onMessage.listen(_handleForegroundMessage);


  ///     !!!!!!!!!!! there is no need for this now , because we implemented the FCM 
  // Set up listener for real-time database changes (for UI updates)
  ///setupRealtimeNotificationListener();

  // Subscribe to FCM topic 'alerts' to receive push notifications
  FirebaseMessaging.instance.subscribeToTopic('alerts');


  // Fetch the current user role before starting the app
  String userRole = 'resident'; // Default role
  User? currentUser = FirebaseAuth.instance.currentUser;
  if (currentUser != null) {
    userRole = await _fetchUserRole(currentUser.uid);
  }
  runApp(MyApp(userRole: userRole));
}



class MyApp extends StatelessWidget {

  final String userRole;

  const MyApp({super.key, required this.userRole});


  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: true, // After debugging turn it to false
      title: 'Smart Doorbell',
      theme: ThemeData(
        colorScheme:
            ColorScheme.fromSeed(seedColor: const Color.fromARGB(255, 15, 169, 1)),
        useMaterial3: true,
      ),
      initialRoute: '/auth',
      routes: {
        '/auth': (context) => const AuthWrapper(),
        '/login': (context) => LoginScreen(),
        '/home': (context) => const AuthWrapper(), // Changed to AuthWrapper for role-based navigation
        '/settings/residents': (context) => ManageResidentsScreen(userRole: userRole),
        '/settings/passwords': (context) => PasswordManagementScreen(userRole: userRole),
        '/settings/preferences': (context) => AppPreferencesScreen(),
      },
    );
  }
}

/// Determines which screen to show based on the user's authentication state and role.
/// It checks if the user is authenticated and then fetches the user's
/// role from the Firebase Realtime Database to navigate to the appropriate screen.
class AuthWrapper extends StatelessWidget {
  const AuthWrapper({super.key});

  ///
  ///     Stream builder listens to the authentication state changes.
  ///     Reacts to changes in the authentication state (e.g., user logs in or out).
  ///
  @override
  Widget build(BuildContext context) {
    return StreamBuilder<User?>(
      stream: FirebaseAuth.instance.authStateChanges(),
      builder: (context, snapshot) {
        if (snapshot.connectionState == ConnectionState.active) {
          final user = snapshot.data;
          if (user == null) {
            return LoginScreen();
          } else {
            // User is logged in, fetch their role
            return FutureBuilder<String>(
              future: _fetchUserRole(user.uid),
              builder: (context, roleSnapshot) {
                if (roleSnapshot.connectionState == ConnectionState.waiting) {
                  return const Center(child: CircularProgressIndicator());
                }

                if (roleSnapshot.hasError) {
                  return const Center(child: Text('Error fetching user role'));
                }

                final userRole = roleSnapshot.data ?? 'resident';
                return AppNavigator(userRole: userRole); // Pass the user role
              },
            );
          }
        }
        return const Center(child: CircularProgressIndicator());
      },
    );
  }
}


