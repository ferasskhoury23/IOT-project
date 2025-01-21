////////////////////////////////////////////////////////////////////////
///
///     Allows the user to log in or register with email and password.
///
///     Once logged in, the user is redirected to the AppNavigator
///
////////////////////////////////////////////////////////////////////////

import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_database/firebase_database.dart';
import '../app_navigator.dart';

class LoginScreen extends StatefulWidget {
  @override
  _LoginScreenState createState() => _LoginScreenState();
}

class _LoginScreenState extends State<LoginScreen> {
  final FirebaseAuth _auth = FirebaseAuth.instance; // Firebase Auth instance
  final FirebaseDatabase _database = FirebaseDatabase.instance; // Firebase Database instance
  final TextEditingController _emailController = TextEditingController();
  final TextEditingController _passwordController = TextEditingController();
  final TextEditingController _nameController = TextEditingController(); // Name controller


  bool _isLoading = false;
  String _errorMessage = "";

  Future<void> _login() async {
    // Input validation
    if (_emailController.text.trim().isEmpty || _passwordController.text.trim().isEmpty) {
      setState(() {
        _errorMessage = "Email and password cannot be empty.";
      });
      return;
    }

    setState(() {
      _isLoading = true;
      _errorMessage = "";
    });

    try {
      // Sign in with email and password
      UserCredential userCredential = await _auth.signInWithEmailAndPassword(
        email: _emailController.text.trim(),
        password: _passwordController.text.trim(),
      );

      // Fetch the user role from Firebase
      String uid = userCredential.user!.uid;
      String role = await _fetchUserRole(uid);

      // Navigate to the main app with the role
      Navigator.pushReplacement(
        context,
        MaterialPageRoute(
          builder: (context) => AppNavigator(userRole: role),
        ),
      );
    } catch (e) {
      setState(() {
        _errorMessage = e.toString();
      });
    } finally {
      setState(() {
        _isLoading = false;
      });
    }
  }

  Future<String> _fetchUserRole(String uid) async {
    try {
      // Fetch the role from the database
      DatabaseReference roleRef = _database.ref('users/$uid/role');
      DataSnapshot snapshot = await roleRef.get();

      if (snapshot.exists) {
        return snapshot.value as String;
      }
      else {
        // Default role to 'resident' if no role is defined
        return 'resident';
      }
    }
    catch (e) {
      // Default to 'resident' in case of an error
      return 'resident';
    }
  }

  Future<void> _register() async {
    // Input validation
    if (_emailController.text.trim().isEmpty || _passwordController.text.trim().isEmpty || _nameController.text.trim().isEmpty) {
      setState(() {
        _errorMessage = "All fields are required.";
      });
      return;
    }

    if (_passwordController.text.trim().length < 6) {
      setState(() {
        _errorMessage = "Password must be at least 6 characters.";
      });
      return;
    }

    setState(() {
      _isLoading = true;
      _errorMessage = "";
    });

    try {
      // Register user with email and password
      UserCredential userCredential = await _auth.createUserWithEmailAndPassword(
        email: _emailController.text.trim(),
        password: _passwordController.text.trim(),
      );

      String uid = userCredential.user!.uid;

      // Set default role and save name in Firebase
      await _database.ref('users/$uid').set({
        'role': 'resident',
        'name': _nameController.text.trim(),
      });

      // Navigate to the main app with the default role
      Navigator.pushReplacement(
        context,
        MaterialPageRoute(
          builder: (context) => AppNavigator(userRole: 'resident'),
        ),
      );
    } catch (e) {
      setState(() {
        _errorMessage = e.toString();
      });
    } finally {
      setState(() {
        _isLoading = false;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("Login/Register"),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            TextField(
              controller: _nameController,
              decoration: const InputDecoration(
                labelText: "Name",
                border: OutlineInputBorder(),
              ),
            ),
            const SizedBox(height: 16),
            TextField(
              controller: _emailController,
              decoration: const InputDecoration(
                labelText: "Email",
                border: OutlineInputBorder(),
              ),
            ),
            const SizedBox(height: 16),
            TextField(
              controller: _passwordController,
              decoration: const InputDecoration(
                labelText: "Password",
                border: OutlineInputBorder(),
              ),
              obscureText: true,
            ),
            const SizedBox(height: 16),
            if (_isLoading)
              const CircularProgressIndicator()
            else ...[
              ElevatedButton(
                onPressed: _isLoading ? null : _register,
                child: const Text("Register"),
              ),
              ElevatedButton(
                onPressed: _isLoading ? null : _login,
                child: const Text("Login"),
              ),
            ],
            const SizedBox(height: 16),
            if (_errorMessage.isNotEmpty)
              Text(
                _errorMessage,
                style: const TextStyle(color: Colors.red),
              ),
          ],
        ),
      ),
    );
  }
}
