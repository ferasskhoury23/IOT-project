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

  @override
  void initState() {
    super.initState();
    _fetchImageFromFirebase(); // Fetch image on initialization
  }

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

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("Live Camera Feed"),
      ),
      body: _imageBytes != null
          ? Center(
              child: Image.memory(_imageBytes!), // Display the decoded image
            )
          : Center(
              child: CircularProgressIndicator(), // Show loading indicator while fetching
            ),
      floatingActionButton: FloatingActionButton(
        onPressed: _fetchImageFromFirebase, // Fetch the latest image on button press
        child: Icon(Icons.refresh),
      ),
    );
  }
}