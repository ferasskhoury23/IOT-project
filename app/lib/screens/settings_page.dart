///
///
///     represents the settings screen of the app.
///     Takes a userRole parameter to customize the settings options based on the user's role.
///
///


import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_database/firebase_database.dart';
import 'settings/password_management_page.dart';
import 'settings/manage_residents_page.dart';

class SettingsScreen extends StatefulWidget {
  final String userRole; 

  const SettingsScreen({super.key, required this.userRole}); // Constructor with userRole

  @override
  _SettingsScreenState createState() => _SettingsScreenState();
}

class _SettingsScreenState extends State<SettingsScreen> {
  final FirebaseAuth _auth = FirebaseAuth.instance;
  final DatabaseReference _database = FirebaseDatabase.instance.ref();
  final TextEditingController _nameController = TextEditingController();

  bool _isLoading = false;
  String _errorMessage = "";

  @override
  void initState() {
    super.initState();
    _fetchUserName();
  }

  Future<void> _fetchUserName() async {
    String uid = _auth.currentUser!.uid;
    try {
      DataSnapshot snapshot = await _database.child('users/$uid/name').get();
      if (snapshot.exists) {
        setState(() {
          _nameController.text = snapshot.value as String;
        });
      }
      else {
        setState(() {
          _nameController.text = "Unknown User";
        });
      }
    }
    catch (e) {
      setState(() {
        _errorMessage = "Failed to fetch name.";
      });
    }
  }

  Future<void> _updateName() async {
    if (_nameController.text.trim().isEmpty) {
      setState(() {
        _errorMessage = "Name cannot be empty.";
      });
      return;
    }

    setState(() {
      _isLoading = true;
      _errorMessage = "";
    });

    try {
      String uid = _auth.currentUser!.uid;
      await _database.child('users/$uid/name').set(_nameController.text.trim());
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text("Name updated successfully!")),
      );
    }
    catch (e) {
      setState(() {
        _errorMessage = "Failed to update name.";
      });
    }
    finally {
      setState(() {
        _isLoading = false;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: ListView(
        padding: const EdgeInsets.all(16.0),
        children: [
          const Text(
            'Settings',
            style: TextStyle(fontSize: 28, fontWeight: FontWeight.bold),
          ),
          const SizedBox(height: 16),

          // Name Editing Section
          const Text(
            'Edit Profile',
            style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold),
          ),
          const SizedBox(height: 16),
          TextField(
            controller: _nameController,
            decoration: const InputDecoration(
              labelText: "Name",
              border: OutlineInputBorder(),
            ),
          ),
          const SizedBox(height: 16),
          if (_isLoading)
            const CircularProgressIndicator()
          else
            ElevatedButton(
              onPressed: _updateName,
              child: const Text("Update Name"),
            ),
          if (_errorMessage.isNotEmpty)
            Padding(
              padding: const EdgeInsets.only(top: 16.0),
              child: Text(
                _errorMessage,
                style: const TextStyle(color: Colors.red),
              ),
            ),
          const Divider(),

          // Manage Home Residents (Available to all users but with role-based restrictions)
          ListTile(
            leading: const Icon(Icons.group),
            title: const Text('Manage Home Residents'),
            subtitle: const Text('View or manage family members'),
            trailing: const Icon(Icons.arrow_forward),
            onTap: () {
              Navigator.push(
                context,
                MaterialPageRoute(
                  builder: (context) => ManageResidentsScreen(userRole: widget.userRole),
                ),
              );
            },
          ),
          const Divider(),

          // Password Management (Available to all users but with role-based restrictions)
          ListTile(
            leading: const Icon(Icons.lock),
            title: const Text('Password Management'),
            subtitle: const Text('View or manage passwords'),
            trailing: const Icon(Icons.arrow_forward),
            onTap: () {
              Navigator.push(
                context,
                MaterialPageRoute(
                  builder: (context) => PasswordManagementScreen(userRole: widget.userRole),
                ),
              );
            },
          ),
          const Divider(),

          // App Preferences (Available to all users)
          ListTile(
            leading: const Icon(Icons.settings),
            title: const Text('App Preferences'),
            subtitle: const Text('Customize your app experience'),
            trailing: const Icon(Icons.arrow_forward),
            onTap: () {
              Navigator.pushNamed(context, '/settings/preferences');
            },
          ),
          const Divider(),
        ],
      ),
    );
  }
}

