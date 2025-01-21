import 'dart:async';
import 'dart:convert';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';

class LiveCameraScreen extends StatefulWidget {
  @override
  _LiveCameraScreenState createState() => _LiveCameraScreenState();
}

class _LiveCameraScreenState extends State<LiveCameraScreen> {
  final DatabaseReference _dbRef = FirebaseDatabase.instance.ref(); // Firebase Database reference
  Uint8List? _imageBytes; // To store the decoded image bytes
  Timer? _refreshTimer; // Timer to auto-refresh the image

  @override
  void initState() {
    super.initState();
    _fetchImageFromFirebase(); // Initial fetch
    _startAutoRefresh(); // Start auto-refresh
  }

  /// Function to start the auto-refresh timer
  void _startAutoRefresh() {
    _refreshTimer = Timer.periodic(const Duration(seconds: 5), (timer) {
      _fetchImageFromFirebase();
    });
  }

  /// Function to fetch the image from Firebase
  Future<void> _fetchImageFromFirebase() async {
    try {
      final snapshot = await _dbRef.child('encoded_image').get(); // Fetch Base64 string
      if (snapshot.exists) {
        String base64String = snapshot.value.toString(); // Retrieve the string value
        setState(() {
          _imageBytes = base64Decode(base64String); // Decode Base64 to image bytes
        });
      } else {
        print("No data available in Firebase.");
      }
    } catch (e) {
      print("Error fetching image from Firebase: $e");
    }
  }

  /// Cancel the timer when the widget is disposed to avoid memory leaks
  @override
  void dispose() {
    _refreshTimer?.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: _imageBytes != null
          ? SizedBox.expand(
              child: Image.memory(
                _imageBytes!,
                fit: BoxFit.cover, // Make the image cover the entire screen
              ),
            )
          : const Center(
              child: CircularProgressIndicator(), // Show loading indicator while fetching
            ),
    );
  }
}
