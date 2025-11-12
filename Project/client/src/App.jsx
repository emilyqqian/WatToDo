import React, { useState } from 'react';
import { BrowserRouter as Router, Routes, Route, Link } from 'react-router-dom';
import './App.css';
import Home from './Pages/Home.jsx';
import Profile from './Pages/Profile.jsx';
import NavBar from './Components/NavBar.jsx';

const TEST_USER = {
  username: 's969chen',
  userId: 1,
  passwordHash: '676767',
  xp: 0,
};

function App() {
  const [user] = useState(TEST_USER);

  return (
    <Router>
      <div className="app-shell">
        <NavBar />
        <main className="app-main">
          <Routes>
            <Route path="/" element={<Home />} />
            <Route path="/profile" element={<Profile user={user} />} />
          </Routes>
        </main>
      </div>
    </Router>
  );
}

export default App;
