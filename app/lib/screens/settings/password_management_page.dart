///
/// Provides the user interface for managing passwords.
/// Responsible for removing expired passwords:
/// Iterates through temporary passwords and removes outdated ones.
///

import 'package:firebase_auth/firebase_auth.dart';
import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/services.dart';

class PasswordManagementScreen extends StatefulWidget {
  final String userRole; // Added userRole parameter

  const PasswordManagementScreen({super.key, required this.userRole});

  @override
  _PasswordManagementScreenState createState() =>
      _PasswordManagementScreenState();
}

class _PasswordManagementScreenState extends State<PasswordManagementScreen> {
  final DatabaseReference _database = FirebaseDatabase.instance.ref();

  String _mainPassword = "Loading...";
  Map<dynamic, dynamic> _tempPasswords = {};

  @override
  void initState() {
    super.initState();
    _fetchPasswords();
  }

  void _fetchPasswords() {
    // Listen to main password
    _database.child("main_password").onValue.listen((event) {
      setState(() {
        _mainPassword = event.snapshot.value.toString();
      });
    });

    // Listen to temporary passwords and remove expired ones
    _database.child("temp_password").onValue.listen((event) {
      final data = event.snapshot.value as Map<dynamic, dynamic>? ?? {};
      final now = DateTime.now();

      // Iterate over temporary passwords
      data.forEach((key, value) {
        final expiresAt = DateTime.tryParse(value['expires_at'] ?? '');
        if (expiresAt != null && expiresAt.isBefore(now)) {
          // Remove expired password from the database
          _database.child("temp_password/$key").remove();
        }
      });

      // Update the remaining passwords in the state
      setState(() {
        _tempPasswords = data..remove("placeholder");
      });
    });
  }

  void _changeMainPassword() {
    final TextEditingController _controller = TextEditingController();

    showDialog(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: const Text('Change Main Password'),
          content: TextField(
            controller: _controller,
            decoration: const InputDecoration(
              hintText: 'Enter new numeric password',
            ),
            keyboardType: TextInputType.number,
            inputFormatters: [
              FilteringTextInputFormatter.digitsOnly,
            ],
            obscureText: true,
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.pop(context),
              child: const Text('Cancel'),
            ),
            TextButton(
              onPressed: () async {
                final newPassword = _controller.text.trim();
                if (newPassword.isNotEmpty) {
                  final user = FirebaseAuth.instance.currentUser;
                  final userId = user?.uid ?? "unknown";
                  final userSnapshot =
                      await _database.child("users/$userId/name").get();
                  final userName = userSnapshot.value?.toString() ?? "Someone";

                  // Update the main password
                  await _database.child("main_password").set(newPassword);

                  // Add log entry
                  await _database.child("logs").push().set({
                    "timestamp": DateTime.now().toIso8601String(),
                    "description":
                        "$userName changed the main password to: $newPassword",
                  });

                  Navigator.pop(context);
                }
              },
              child: const Text('Save'),
            ),
          ],
        );
      },
    );
  }

  void _addTemporaryPassword() {
    final TextEditingController _passwordController = TextEditingController();
    DateTime? selectedDate;

    showDialog(
      context: context,
      builder: (context) {
        return StatefulBuilder(
          builder: (context, setState) {
            return AlertDialog(
              title: const Text('Add Temporary Password'),
              content: Column(
                mainAxisSize: MainAxisSize.min,
                children: [
                  TextField(
                    controller: _passwordController,
                    decoration: const InputDecoration(
                      hintText: 'Enter numeric password',
                    ),
                    keyboardType: TextInputType.number,
                    inputFormatters: [
                      FilteringTextInputFormatter.digitsOnly,
                    ],
                  ),
                  const SizedBox(height: 10),
                  TextButton(
                    onPressed: () async {
                      final pickedDate = await showDatePicker(
                        context: context,
                        initialDate: DateTime.now(),
                        firstDate: DateTime.now(),
                        lastDate: DateTime.now().add(const Duration(days: 365)),
                      );

                      if (pickedDate != null) {
                        final pickedTime = await showTimePicker(
                          context: context,
                          initialTime: TimeOfDay.now(),
                        );

                        if (pickedTime != null) {
                          final fullDateTime = DateTime(
                            pickedDate.year,
                            pickedDate.month,
                            pickedDate.day,
                            pickedTime.hour,
                            pickedTime.minute,
                          );
                          setState(() {
                            selectedDate = fullDateTime;
                          });
                        }
                      }
                    },
                    child: Text(
                      selectedDate == null
                          ? 'Pick Expiration Date'
                          : 'Selected: ${selectedDate.toString()}',
                      style: const TextStyle(fontSize: 16),
                    ),
                  ),
                ],
              ),
              actions: [
                TextButton(
                  onPressed: () => Navigator.pop(context),
                  child: const Text('Cancel'),
                ),
                TextButton(
                  onPressed: () async {
                    final password = _passwordController.text.trim();
                    if (password.isEmpty || selectedDate == null) {
                      ScaffoldMessenger.of(context).showSnackBar(
                        const SnackBar(
                            content: Text('Please fill all fields')),
                      );
                      return;
                    }

                    final user = FirebaseAuth.instance.currentUser;
                    final userId = user?.uid ?? "unknown";
                    final userSnapshot =
                        await _database.child("users/$userId/name").get();
                    final userName =
                        userSnapshot.value?.toString() ?? "Someone";

                    // Save the temporary password to Firebase
                    final expiresAt = selectedDate!.toIso8601String();
                    await _database.child("temp_password").push().set({
                      "password": password,
                      "expires_at": expiresAt,
                      "name": userName,
                    });

                    // Add log entry
                    await _database.child("logs").push().set({
                      "timestamp": DateTime.now().toIso8601String(),
                      "description":
                          "$userName added a temporary password: $password (Expires: $expiresAt)",
                    });

                    Navigator.pop(context);
                  },
                  child: const Text('Add'),
                ),
              ],
            );
          },
        );
      },
    );
  }

  void _deleteTemporaryPassword(String key) async {
    try {
      await _database.child("temp_password").child(key).remove();
    } catch (e) {
      print("Error deleting temporary password: $e");
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Password Management')),
      body: ListView(
        padding: const EdgeInsets.all(16.0),
        children: [
          // Main Password Section
          Card(
            child: ListTile(
              title: const Text('Main Password'),
              subtitle: Text(_mainPassword),
              trailing: widget.userRole == 'homeowner'
                  ? IconButton(
                      icon: const Icon(Icons.edit),
                      onPressed: _changeMainPassword,
                    )
                  : null, // Hide edit button for residents
            ),
          ),
          const SizedBox(height: 20),

          // Temporary Passwords Section
          const Text(
            'Temporary Passwords',
            style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
          ),
          const SizedBox(height: 10),
          ..._tempPasswords.entries.map((entry) {
            final key = entry.key;
            final data = entry.value;
            return ListTile(
              title: Text('Password: ${data["password"]}'),
              subtitle: Text(
                  'Expires at: ${data["expires_at"]}\nAdded by: ${data["name"]}'),
              trailing: IconButton(
                icon: const Icon(Icons.delete),
                onPressed: () => _deleteTemporaryPassword(key),
              ),
            );
          }).toList(),
          const SizedBox(height: 10),
          ElevatedButton.icon(
            icon: const Icon(Icons.add),
            label: const Text('Add Temporary Password'),
            onPressed: _addTemporaryPassword,
          ),
        ],
      ),
    );
  }
}
