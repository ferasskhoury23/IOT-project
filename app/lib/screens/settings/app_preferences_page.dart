import 'package:flutter/material.dart';
import 'package:firebase_messaging/firebase_messaging.dart';
import 'package:shared_preferences/shared_preferences.dart';

class AppPreferencesScreen extends StatefulWidget {
  @override
  _AppPreferencesScreenState createState() => _AppPreferencesScreenState();
}

class _AppPreferencesScreenState extends State<AppPreferencesScreen> {
  bool _notificationsEnabled = true; // Default value

  @override
  void initState() {
    super.initState();
    _loadPreferences(); // Load saved preferences on startup
  }

  Future<void> _loadPreferences() async {
    final prefs = await SharedPreferences.getInstance();
    setState(() {
      _notificationsEnabled = prefs.getBool('notificationsEnabled') ?? true;
    });
  }

  Future<void> _toggleNotifications(bool value) async {
    setState(() {
      _notificationsEnabled = value;
    });

    final prefs = await SharedPreferences.getInstance();
    await prefs.setBool('notificationsEnabled', value);

    if (_notificationsEnabled) {
      await FirebaseMessaging.instance.subscribeToTopic('alerts');
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text("Notifications enabled")),
      );
    } else {
      await FirebaseMessaging.instance.unsubscribeFromTopic('alerts');
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text("Notifications disabled")),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('App Preferences')),
      body: ListView(
        padding: const EdgeInsets.all(16.0),
        children: [
          SwitchListTile(
            title: const Text('Enable Notifications'),
            value: _notificationsEnabled,
            onChanged: _toggleNotifications,
          ),
          const Divider(),
        ],
      ),
    );
  }
}
