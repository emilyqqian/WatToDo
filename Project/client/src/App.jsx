import React, { useState, useEffect } from 'react'
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom'
import { Box } from '@mui/material'
import './App.css'
import Home from './Pages/Home.jsx'
import Profile from './Pages/Profile.jsx'
import NavBar from './Components/NavBar.jsx'

// optional: default placeholder user while loading
const DEFAULT_USER = {
  username: 'Loading...',
  userId: null,
  passwordHash: '',
  xp: 0,
}

function App() {
  const [user, setUser] = useState(DEFAULT_USER)
  const [loaded, setLoaded] = useState(false)

  useEffect(() => {
    async function getUser() {
      try {
        const response = await fetch('http://0.0.0.0:18080/users/s969chen')
        console.log("a")
        const data = await response.json()
        console.log("b")
        setUser(data)        // 🔥 this tells React to re-render with new user
        console.log("c")
      } catch (err) {
        console.error('Failed to fetch user:', err)
      } finally {
        setLoaded(true)
        console.log("d")
      }
    }

    getUser()
  }, []) // empty deps = run once when App mounts

  if (!loaded) {
    return <p>Loading user...</p>
  }

  return (
    <Router>
      <Box
        sx={{
          minHeight: '100vh',
          display: 'flex',
          flexDirection: 'column',
          backgroundColor: 'background.default',
        }}
      >
        <NavBar />
        <Box
          component="main"
          sx={{
            flex: 1,
            width: '100%',
            py: 4,
          }}
        >
          <Routes>
            <Route path="/" element={<Home />} />
            <Route path="/profile" element={<Profile user={user} />} />
          </Routes>
        </Box>
      </Box>
    </Router>
  )
}

export default App