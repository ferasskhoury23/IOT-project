///
///     provides the user interface for managing home residents.
///     Homeowners can add, edit, and remove residents.
///     Residents can only view the list of users and their roles.
///

import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:firebase_auth/firebase_auth.dart';

class ManageResidentsScreen extends StatefulWidget {
  final String userRole; // User role passed from the Settings page

  const ManageResidentsScreen({super.key, required this.userRole});

  @override
  _ManageResidentsScreenState createState() => _ManageResidentsScreenState();
}

class _ManageResidentsScreenState extends State<ManageResidentsScreen> {
  final DatabaseReference _database = FirebaseDatabase.instance.ref();
  final String _currentUserId = FirebaseAuth.instance.currentUser!.uid;
  List<Map<String, String>> _users = []; // List to store users with name and role
  bool _isLoading = true;

  @override
  void initState() {
    super.initState();
    _fetchUsers();
  }

  Future<void> _fetchUsers() async {
    try {
      DataSnapshot snapshot = await _database.child('users').get();
      if (snapshot.exists) {
        List<Map<String, String>> users = [];
        Map<dynamic, dynamic> data = snapshot.value as Map<dynamic, dynamic>;

        data.forEach((uid, userData) {
          users.add({
            'uid': uid,
            'name': userData['name'] ?? 'Unknown',
            'role': userData['role'] ?? 'resident',
          });
        });

        setState(() {
          _users = users;
          _isLoading = false;
        });
      }
    } catch (e) {
      setState(() {
        _isLoading = false;
      });
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text("Failed to fetch users")),
      );
    }
  }

  Future<void> _confirmAndUpdateUserRole(String uid, bool isHomeowner) async {
    if (widget.userRole != 'homeowner') return; // Prevent role changes if not homeowner

    String newRole = isHomeowner ? 'homeowner' : 'resident';
    bool confirmed = await _showConfirmationDialog(newRole);

    if (confirmed) {
      try {
        await _database.child('users/$uid/role').set(newRole);
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text("Role updated to $newRole")),
        );
        setState(() {
          _users = _users.map((user) {
            if (user['uid'] == uid) {
              user['role'] = newRole;
            }
            return user;
          }).toList();
        });
      } catch (e) {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(content: Text("Failed to update role")),
        );
      }
    }
  }

  Future<bool> _showConfirmationDialog(String newRole) async {
    return await showDialog<bool>(
          context: context,
          builder: (context) {
            return AlertDialog(
              title: const Text("Confirm Role Change"),
              content: Text("Are you sure you want to change the role to $newRole?"),
              actions: [
                TextButton(
                  onPressed: () => Navigator.of(context).pop(false), // Cancel
                  child: const Text("Cancel"),
                ),
                TextButton(
                  onPressed: () => Navigator.of(context).pop(true), // Confirm
                  child: const Text("Confirm"),
                ),
              ],
            );
          },
        ) ??
        false; // Default to false if dialog is dismissed
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("Manage Home Residents"),
      ),
      body: _isLoading
          ? const Center(child: CircularProgressIndicator())
          : ListView.builder(
              padding: const EdgeInsets.all(16.0),
              itemCount: _users.length,
              itemBuilder: (context, index) {
                final user = _users[index];
                final bool isHomeowner = user['role'] == 'homeowner';
                final bool isCurrentUser = user['uid'] == _currentUserId;

                return ListTile(
                  leading: const Icon(Icons.person),
                  title: Text(user['name']!),
                  subtitle: Text("Role: ${user['role']}"),
                  trailing: isCurrentUser
                      ? const Text("You", style: TextStyle(fontWeight: FontWeight.bold))
                      : widget.userRole == 'homeowner'
                          ? Switch(
                              value: isHomeowner,
                              onChanged: (value) {
                                _confirmAndUpdateUserRole(user['uid']!, value);
                              },
                            )
                          : const Text("View Only"), // Residents can only view roles
                );
              },
            ),
    );
  }
}
