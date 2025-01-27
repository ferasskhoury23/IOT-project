///
///
///     Represents the home screen of the app.
///     Takes a userRole parameter to customize the home screen based on the user's role.
///
///

import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';

class Home extends StatefulWidget {
  final String userRole; // Add userRole parameter

  const Home({super.key, required this.userRole}); // Constructor with userRole

  @override
  _HomeState createState() => _HomeState();
}

class _HomeState extends State<Home> {
  final DatabaseReference _database = FirebaseDatabase.instance.ref();
  String _doorStatus = "Unknown"; // Default door status
  String _currentMessage = "Fetching..."; // Default current message
  final TextEditingController _messageController = TextEditingController(); // Controller for display message

  List<Map<String, dynamic>> _logs = []; // Store logs

  @override
  void initState() {
    super.initState();
    _fetchDoorStatusAndMessage();
    _fetchLogs();
  }

  void _fetchDoorStatusAndMessage() {
    // Fetch current door status
    _database.child("lock_state").once().then((event) {
      setState(() {
        _doorStatus = event.snapshot.value?.toString() ?? "Unknown";
      });
    });

    _database.child("lock_state").onValue.listen((event) {
      setState(() {
        _doorStatus = event.snapshot.value?.toString() ?? "Unknown";
      });
    });

    // Fetch current display message
    _database.child("display_message").onValue.listen((event) {
      setState(() {
        _currentMessage = event.snapshot.value?.toString() ?? "No message set";
      });
    });
  }

  void _fetchLogs() {
    _database.child("logs").orderByChild("timestamp").limitToLast(20).onValue.listen((event) {
      final data = event.snapshot.value as Map<dynamic, dynamic>? ?? {};
      final List<Map<String, dynamic>> fetchedLogs = data.entries.map((entry) {
        return {
          "timestamp": entry.value["timestamp"],
          "description": entry.value["description"],
        };
      }).toList();

      setState(() {
        _logs = fetchedLogs.reversed.toList(); // Show newest logs first
      });
    });
  }

  void _clearLogs() {
    _database.child("logs").remove().then((_) {
      setState(() {
        _logs.clear(); // Clear the logs locally
      });
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text("Logs cleared successfully!")),
      );
    }).catchError((error) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text("Failed to clear logs: $error")),
      );
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          // Door Status
          Padding(
            padding: const EdgeInsets.all(16.0),
            child: Text(
              "Door Status: $_doorStatus",
              textAlign: TextAlign.center,
              style: const TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
            ),
          ),

          // Lock/Unlock Buttons
          Padding(
            padding: const EdgeInsets.all(16.0),
            child: Row(
              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
              children: [
                ElevatedButton(
                  /// Lock door button
                  onPressed: () {
                    if (_doorStatus != "Locked") {
                      setState(() {
                        _doorStatus = "Locked";
                      });
                      _database.child("lock_state").set("locked");
                      ScaffoldMessenger.of(context).showSnackBar(
                        const SnackBar(content: Text("Door locked successfully!")),
                      );
                    } else {
                      ScaffoldMessenger.of(context).showSnackBar(
                        const SnackBar(content: Text("Door is already locked!")),
                      );
                    }
                  },
                  child: const Text(
                    "Lock Door",
                    style: TextStyle(fontSize: 18),
                  ),
                ),
                ElevatedButton(
                  /// Unlock door button
                  onPressed: () {
                    if (_doorStatus != "Unlocked") {
                      setState(() {
                        _doorStatus = "Unlocked";
                      });
                      _database.child("lock_state").set("unlocked");
                      ScaffoldMessenger.of(context).showSnackBar(
                        const SnackBar(content: Text("Door unlocked successfully!")),
                      );
                    } else {
                      ScaffoldMessenger.of(context).showSnackBar(
                        const SnackBar(content: Text("Door is already unlocked!")),
                      );
                    }
                  },
                  style: ElevatedButton.styleFrom(
                    padding: const EdgeInsets.symmetric(vertical: 16.0, horizontal: 24.0),
                  ),
                  child: const Text(
                    "Unlock Door",
                    style: TextStyle(fontSize: 18),
                  ),
                ),
              ],
            ),
          ),

          // Display Message Section
          Padding(
            padding: const EdgeInsets.all(16.0),
            child: Column(
              children: [
                TextField(
                  controller: _messageController,
                  decoration: const InputDecoration(
                    border: OutlineInputBorder(),
                    labelText: "Enter Message",
                  ),
                ),
                const SizedBox(height: 10),
                ElevatedButton(
                  onPressed: () {
                    String message = _messageController.text.trim();
                    if (message.isNotEmpty) {
                      _database.child("display_message").set(message);
                      ScaffoldMessenger.of(context).showSnackBar(
                        const SnackBar(content: Text("Message updated successfully!")),
                      );
                    } else {
                      ScaffoldMessenger.of(context).showSnackBar(
                        const SnackBar(content: Text("Message cannot be empty!")),
                      );
                    }
                  },
                  child: const Text("Update Message"),
                ),
                const SizedBox(height: 10),
                Text(
                  "Current Message: $_currentMessage",
                  style: const TextStyle(fontSize: 16, color: Colors.grey),
                ),
              ],
            ),
          ),

          // Logs Area with Clear Button
          Expanded(
            flex: 3,
            child: Column(
              children: [
                // Clear Logs Button
                Padding(
                  padding: const EdgeInsets.symmetric(horizontal: 16.0),
                  child: ElevatedButton(
                    onPressed: _clearLogs,
                    child: const Text("Clear Logs"),
                  ),
                ),
                Expanded(
                  child: ListView.builder(
                    itemCount: _logs.length,
                    itemBuilder: (context, index) {
                      final log = _logs[index];
                      return ListTile(
                        leading: const Icon(Icons.event),
                        title: Text(log["description"]),
                        subtitle: Text(log["timestamp"]),
                      );
                    },
                  ),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}
