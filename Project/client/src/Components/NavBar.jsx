import React from 'react';
import { Link } from 'react-router-dom';

function NavBar() {
  return (
    <nav className="top-nav">
      <h1 className="app-title">Wat To Do</h1>
      <div className="nav-links">
        <Link to="/">Home</Link>
        <Link to="/profile">Profile</Link>
      </div>
    </nav>
  );
}

export default NavBar;
